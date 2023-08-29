#include "Server.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

// Core Functions

void Server::run() {
  std::cout << "\n*** Listening on ADDRESS: "
            << this->sockets_[0].getAddressString()
            << " PORT: " << this->sockets_[0].getPort() << " ***\n\n";
  while (g_signaled == 0) {
    if (poll(this->pollfds_, this->numfds_, MAX_PORTS) == -1) {
      perror("poll");
      exit(EXIT_FAILURE);
    }
    checkSocketTimeout();
    for (size_t i = 0; i < this->numfds_; ++i) {
      if (this->pollfds_[i].fd <= 0) {
        continue;
      }
      if (this->pollfds_[i].revents & POLLIN) {
        if (this->sockets_[i].getState() == SERVER) {
          newConnection();
          break;
        } else {
          handleRecieve(i);
          break;
        }
      } else if (this->pollfds_[i].revents & POLLOUT) {
        handleSend(i);
      } else {
        pollError(i);
      }
    }
    if (PRINT) {
      std::cout << this->numfds_ << std::endl;
    }
    while (this->pollfds_[this->numfds_ - 1].fd == -1) {
      this->numfds_--;
    }
  }
}

int Server::startServer(int port) {
  pollfd new_poll;
  Socket serv(0);
  int opt = 1;
  socklen_t addrlen = sizeof(serv.getAddress());

  this->numfds_ = 1;
  memset(pollfds_, -1, MAX_PORTS);
  new_poll.fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.setState(SERVER);
  if (new_poll.fd < 0) {
    std::cout << "Error: can not start Socket." << std::endl;
    return EXIT_FAILURE;
  }
  serv.setPort(port);
  if (setsockopt(new_poll.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    std::cerr << "Error: cannot set socket opt." << std::endl;
  }
  if (bind(new_poll.fd, (struct sockaddr*)&serv.getAddress(), addrlen)) {
    std::cerr << "Error: cannot bind to address." << std::endl;
    return EXIT_FAILURE;
  }
  if (listen(new_poll.fd, QUEUE_LEN) == -1) {
    std::cerr << "Error: failed to serv on connection" << std::endl;
    return EXIT_FAILURE;
  }
  if (fcntl(new_poll.fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
    std::cerr << "Error: fcntl" << std::endl;
    return EXIT_FAILURE;
  }
  new_poll.events = POLLIN;
  new_poll.revents = 0;
  this->sockets_[0] = serv;
  this->pollfds_[0] = new_poll;
  return EXIT_SUCCESS;
}

// private member Functions

void Server::handleSend(int i) {
  sendResponse(i);
  if (!this->sockets_[i].isKeepalive() &&
      this->sockets_[i].getState() != UNFINISHED) {
    std::cout << "Closing socket" << std::endl;
    removeFd(i);
  }
}

void Server::handleRecieve(int i) {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t rec = 0;

  std::cout << "== Connected on socket: " << this->pollfds_[i].fd
            << " ==" << std::endl
            << std::endl;
  rec = recv(this->pollfds_[i].fd, buffer, BUFFER_SIZE, O_NONBLOCK);
  if (rec < 0) {
    std::cerr << "Error: Failed to read bytes from client socket connection"
              << std::endl;
    removeFd(i);
    return;
  } else if (rec == 0) {
    std::cerr << "Error: Client closed connection." << std::endl;
    removeFd(i);
    return;
  }
  std::string stringyfied_buff(buffer, rec);
  std::cout << "rec: " << rec << std::endl;
  std::cout << "buffer.size(): " << stringyfied_buff.size() << std::endl;
  if (this->sockets_[i].getRequest().getContentLength() >
      this->sockets_[i].getRequest().getBody().size()) {
    std::cout << "APPENDING" << std::endl;
    this->sockets_[i].getRequest().appendBody(stringyfied_buff);
    std::cout << "CL: " << this->sockets_[i].getRequest().getContentLength() << std::endl;
    std::cout << "BL: " << this->sockets_[i].getRequest().getBody().size() << std::endl;
    this->pollfds_[i].events = POLLIN;
    if (this->sockets_[i].getRequest().getContentLength() <=
        this->sockets_[i].getRequest().getBody().size()) {
      std::cout << "Done Appending" << std::endl;
      this->sockets_[i].setState(SEND);
      this->pollfds_[i].events = POLLOUT;
    }
    return;
  } else {
    try {
      HTTPRequest req(stringyfied_buff);
      this->sockets_[i].setRequest(req);
      std::cout << "Recieved from socket: " << pollfds_[i].fd << std::endl;
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
}

void Server::sendResponse(int i) {
  int bytes_sent = 0;
  std::string res_string;
  if (this->sockets_[i].getState() == UNFINISHED) {
    res_string = this->sockets_[i].getResponse();
  } else {
    HTTPResponse res(this->sockets_[i].getRequest(), this->settings_);
    res_string = res.toString();
  }
  bytes_sent =
      send(this->pollfds_[i].fd, res_string.data(), res_string.size(), 0);
  if (bytes_sent < 0) {
    std::cout << "Error: sending response to client" << std::endl;
  } else if (static_cast<size_t>(bytes_sent) < res_string.size()) {
    this->sockets_[i].handleUnfinished(bytes_sent, res_string);
    this->pollfds_[i].events = POLLOUT;
  } else {
    this->sockets_[i].setState(RECEIVE);
    this->pollfds_[i].events = POLLIN;
  }
  this->sockets_[i].updateTime();
  std::cout << "Sending to Socket: " << pollfds_[i].fd << " of size "
            << bytes_sent << std::endl;
}

void Server::newConnection() {
  if (this->numfds_ > MAX_PORTS) {
    std::cout << "Error: no new Connection possible." << std::endl;
    return;
  }
  Socket new_client;
  socklen_t addrlen = sizeof(struct sockaddr);
  pollfd new_poll;
  size_t index = searchFreePoll();
  std::cout << index << std::endl;
  new_poll.fd = accept(this->pollfds_[0].fd,
                       (struct sockaddr*)&new_client.getAddress(), &addrlen);
  if (new_poll.fd < 0) {
    std::cout << "Error: Failed to accept connection." << std::endl;
  }
  new_poll.events = POLLIN;
  new_poll.revents = 0;
  new_client.setState(RECEIVE);
  new_client.setIndex(index);
  this->sockets_[index] = new_client;
  this->pollfds_[index] = new_poll;
  if (index == this->numfds_) {
    numfds_++;
  }
  std::cout << "New connection success on : " << new_client.getAddressString()
            << " with socket nbr: " << new_poll.fd << std::endl;
}

size_t Server::searchFreePoll() {
  for (size_t i = 0; i < this->numfds_; ++i) {
    if (this->pollfds_[i].fd == -1) {
      return i;
    }
  }
  return this->numfds_;
}

int Server::pollError(int i) {
  int res = 0;
  if (this->pollfds_[i].revents & POLLERR) {
    std::cout << "Error: POLLERR" << std::endl;
    res = 1;
  } else if (this->pollfds_[i].revents & POLLPRI) {
    std::cout << "Error: POLLPRI" << std::endl;
    res = 1;
  } else if (this->pollfds_[i].revents & POLLNVAL) {
    std::cout << "Error: POLLNVAL" << std::endl;
    res = 1;
  } else if (this->pollfds_[i].revents & POLLHUP) {
    std::cout << "Error: POLLHUP" << std::endl;
    res = 1;
  }
  if (res != 0) {
    std::cout << "pollfd: " << this->pollfds_[i].fd << std::endl;
    removeFd(i);
  }
  return res;
}

void Server::removeFd(int i) {
  std::cout << "removing fd: " << this->pollfds_[i].fd << std::endl;
  shutdown(this->pollfds_[i].fd, SHUT_RDWR);
  close(this->pollfds_[i].fd);
  this->pollfds_[i].fd = -1;
  this->pollfds_[i].events = POLLERR;
  this->sockets_.erase(i);
}

void Server::checkSocketTimeout() {
  std::map<int, Socket>::iterator end = this->sockets_.end();
  for (std::map<int, Socket>::iterator it = this->sockets_.begin(); it != end;
       ++it) {
    if (it->second.getState() != SERVER && it->second.checkTimeout()) {
      removeFd(it->second.getIndex());
      break;
    }
  }
}

//// Constructors and Operator overloads

Server::Server(const std::string& ip_addr, int port) : ip_addr_(ip_addr) {
  if (startServer(port) == EXIT_FAILURE) {
    std::cerr << "Error: Failed to start Server." << std::endl;
    exit(EXIT_FAILURE);
  }
}

Server::~Server() { exit(EXIT_SUCCESS); }

Server::Server(const Server& obj) { *this = obj; }

Server& Server::operator=(const Server& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

Server::Server(const Settings& settings) : settings_(settings) {
  if (startServer(this->settings_.getServers()[0].getPort()) != 0) {
    std::cout << "Error: failed to start server with port: "
              << this->settings_.getServers()[0].getPort() << std::endl;
  }
}
