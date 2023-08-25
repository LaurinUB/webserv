#include "Server.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

const int BUFFER_SIZE = 30640;
const int QUEUE_LEN = 40;

void Server::run() {
  std::cout << "\n*** Listening on ADDRESS: "
    << this->sockets_[0].getAddressString()
    << " PORT: " << this->sockets_[0].getPort() << " ***\n\n";
  while (g_signaled == 0) {
    if (poll(this->pollfds_, this->numfds_, 255) == -1) {
      perror("poll");
      exitWithError("Poll failed");
    }
    // checkSocketTimeout();
    // if (this->pollfds_[0].revents & POLLIN) {
    //   std::cout << "POLLIN" << std::endl;
    // }
    // this->sockets_[0].getStringState();
    // std::cout << this->sockets_.size() << std::endl;
    // std::cout << this->sockets_[0].getPoll().events << std::endl;
    // std::cout << this->sockets_[0].getPoll().revents << std::endl;
    // std::cout << this->sockets_[0].getPort() << std::endl;
    // log("====== Waiting for a new connection ======\n\n\n");
    for (size_t i = 0; i < this->numfds_; ++i) {
      // std::cout << this->sockets_[i].getPoll().events << std::endl;
      // std::cout << "revents: "<< this->sockets_[i].getPoll().revents << std::endl;
      // std::cout << "revents: "<< this->pollfds_[i].revents << std::endl;
      // if (PRINT && this->pollfds_[i].fd != 3 && this->pollfds_[i].fd != -1) {
        // std::cout << this->sockets_[i]<< "i: " << i
        //           << std::endl;
        // std::cout << "pollfds_.size(): " << this->pollfds_.size() << std::endl;
        // std::cout << "pollfds_.fd: " << this->pollfds_[i].fd << std::endl;
      //   std::cout << "pollfds.fd: " << this->pollfds_[i].fd << std::endl;
      // }
      // if (this->pollfds_[i].fd <= 0) {
      //   continue;
      // }
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
        pollError(this->pollfds_[i]);
      }
    }
  }
}


void log(const std::string& msg) { std::cout << msg << std::endl; }

void exitWithError(const std::string& errorMsg) {
  log("Error: " + errorMsg);
  exit(EXIT_FAILURE);
}

int Server::startServer(int port) {
  pollfd new_poll;
  memset(pollfds_, -1, 250);
  this->numfds_ = 1;
  new_poll.fd = socket(AF_INET, SOCK_STREAM, 0);
  Socket serv(0);
  serv.setState(SERVER);
  if (new_poll.fd < 0) {
    std::cout << "Error: can not start Socket." << std::endl;
    return EXIT_FAILURE;
  }
  serv.setPort(port);
  int opt = 1;
  socklen_t addrlen = sizeof(serv.getAddress());
  if (setsockopt(new_poll.fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) == -1) {
    std::cerr << "Error: cannot set socket opt." << std::endl;
  }
  std::cout << "Success: Socket opt set." << std::endl;
  if (bind(new_poll.fd, (struct sockaddr*)&serv.getAddress(),
        addrlen)) {
    std::cerr << "Error: cannot bind to address." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Success: Socket bind." << std::endl;
  if (listen(new_poll.fd, QUEUE_LEN) == -1) {
    std::cerr << "Error: failed to serv on connection" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Success: Socket serv." << std::endl;
  if (fcntl(new_poll.fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
    std::cerr << "Error: fcntl" <<std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Success: Socket fcntl." << std::endl;
  new_poll.events = POLLIN;
  new_poll.revents = 0;
  this->sockets_[0] = serv;
  this->pollfds_[0] = new_poll;
  return EXIT_SUCCESS;
}

int Server::pollError(pollfd& poll) {
  int res = 0;
  if (poll.revents & POLLERR) {
    std::cout << "Error: POLLERR" << std::endl;
    res = 1;
  } else if (poll.revents & POLLPRI) {
    std::cout << "Error: POLLPRI" << std::endl;
    res = 1;
  } else if (poll.revents & POLLNVAL) {
    std::cout << "Error: POLLNVAL" << std::endl;
    res = 1;
  } else if (poll.revents & POLLHUP) {
    std::cout << "Error: POLLHUP" << std::endl;
    res = 1;
  }
  if (res != 0) {
    std::cout << "pollfd: " << poll.fd << std::endl;
    removeFd(poll.fd);
  }
  return res;
}

void Server::removeFd(int fd) {
  for (size_t i = 0; i < this->numfds_; ++i) {
    if (this->pollfds_[i].fd == fd) {
      this->pollfds_[i].fd = -1;
      this->sockets_.erase(i);
      shutdown(fd, SHUT_RDWR);
      close(fd);
      break;
    }
  }
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

void Server::handleSend(int i) {
  sendResponse(i);
  if (!this->sockets_[i].isKeepalive()) {
    log("Closing socket");
    removeFd(this->pollfds_[i].fd);
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
    exitWithError("Error: Failed to read bytes from client socket connection");
  } else if (rec == 0) {
    std::cout << "Error: Client closed connection." << std::endl;
    removeFd(i);
    return;
  }
  std::string stringyfied_buff(buffer);
  try {
    HTTPRequest req(stringyfied_buff);
    this->pollfds_[i].events = POLLOUT;
    this->sockets_[i].setRequest(req);
    this->sockets_[i].setState(SEND);
    std::cout << "Recieved from socket: " << pollfds_[i].fd << std::endl;
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void Server::sendResponse(int i) {
  int bytes_sent = 0;
  std::string res_string;
  if (this->sockets_[i].getState() == UNFINISHED) {
    res_string = this->sockets_[i].getResponse();
  } else {
    HTTPResponse res(this->sockets_[i].getRequest());
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
    this->sockets_[i].setState(RECIEVE);
    this->pollfds_[i].events = POLLIN;
  }
  this->sockets_[i].updateTime();
  std::cout << "Sending to Socket: " << pollfds_[i].fd
      << " of size " << bytes_sent << std::endl;
}

size_t Server::searchFreePoll() {
  for (size_t i = 0; i < this->numfds_; ++i) {
    if (this->pollfds_[i].fd == -1) {
      return i;
    }
  }
  return this->numfds_;
}

void Server::newConnection() {
  if (this->numfds_ > 250) {
    std::cout << "Error: no new Connection possible." << std::endl;
    return;
  }
  Socket new_client;
  socklen_t addrlen = sizeof(struct sockaddr);
  pollfd new_poll;
  size_t index = searchFreePoll();
  std::cout << index << std::endl;
  new_poll.fd = accept(this->pollfds_[0].fd,
                          (struct sockaddr*)&new_client.getAddress(),
                          &addrlen);
  if (new_poll.fd < 0) {
    std::cout << "Error: Failed to accept connection." << std::endl;
  }
  new_poll.events = POLLIN;
  new_poll.revents = POLLOUT;
  new_client.setState(RECIEVE);
  new_client.setIndex(index);
  this->sockets_[index] = new_client;
  this->pollfds_[index] = new_poll;
  if (index == this->numfds_) {
    numfds_++;
  }
  std::cout << "New connection success on : " << new_client.getAddressString()
            << " with socket nbr: " << new_poll.fd << std::endl;
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
