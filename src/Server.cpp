#include "Server.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

// Core Functions

void Server::run() {
  while (g_signaled == 0) {
    if (poll(this->pollfds_, this->numfds_, TIMEOUT) == -1) {
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
          newConnection(i);
          break;
        } else {
          handleReceive(i);
          break;
        }
      } else if (this->pollfds_[i].revents & POLLOUT) {
        handleSend(i);
        break;
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

int Server::startServer(std::string ip, int port) {
  pollfd new_poll;
  Socket serv(this->numfds_);
  int opt = 1;
  socklen_t addrlen = sizeof(serv.getAddress());

  new_poll.fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.setState(SERVER);
  if (new_poll.fd < 0) {
    std::cout << "Error: can not start Socket." << std::endl;
    return EXIT_FAILURE;
  }
  serv.setPort(ip, port);
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
  this->sockets_[this->numfds_] = serv;
  this->pollfds_[this->numfds_] = new_poll;
  this->numfds_++;
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

void Server::generateEnv(const HTTPRequest& req) {
  int i = 0;
  std::string env[5];
  if (req.getMethod() == HTTPRequest::GET) {
    env[i] = "REQUEST_METHOD=GET";
    i++;
  } else {
    env[i] = "REQUEST_METHOD=POST";
    i++;
    env[i] = "CONTENT_LENGTH=" + std::to_string(req.getBody().size());
    i++;
  }
  env[i] = "CONTENT_TYPE=text/html";
  env[i] = "SCRIPT_NAME=" + req.getURI();
  int j = 0;
  while (j < i) {
    this->cgi_env_[j] = const_cast<char*>(env[j].c_str());
    j++;
  }
  this->cgi_env_[j] = NULL;
}

void Server::executeCGI(const HTTPRequest& req, int i) {
  std::string executable = req.getLocationSettings().getRoot() + req.getURI();
  char* arguments[3];

  arguments[0] = const_cast<char*>(INTERPRETER);
  arguments[1] = const_cast<char*>(executable.c_str());
  arguments[2] = NULL;
  pid_t child = fork();
  if (child < 0) {
    std::cerr << "Error: fork." << std::endl;
  } else if (child == 0) {
    dup2(this->pollfds_[i].fd, STDOUT_FILENO);
    close(this->pollfds_[i].fd);
    std::cout << "HTTP/1.1 200 OK\n";
    if (execve(arguments[1], arguments, this->cgi_env_) == -1) {
      std::cerr << "Error: execve." << std::endl;
    }
    std::cout << "\nScript execution failed!" << std::endl;
    exit(EXIT_FAILURE);
  }
  int status;
  waitpid(child, &status, 0);
}

bool Server::isCGI(const HTTPRequest& req) {
  if (req.getURI().empty() || req.getURI().size() < PYSIZE + 1) {
    return false;
  }
  if (!req.getURI().compare(req.getURI().size() - PYSIZE, req.getURI().size(),
                            PYTHON)) {
    return true;
  }
  return false;
}

void Server::handleReceive(int i) {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t rec = 0;

  std::cout << "== Connected on socket: " << this->pollfds_[i].fd
            << " ==" << std::endl;
  rec = recv(this->pollfds_[i].fd, buffer, BUFFER_SIZE, O_NONBLOCK);
  if (rec < 0) {
    std::cerr << "Error: Failed reading from client socket" << std::endl;
    removeFd(i);
    return;
  } else if (rec == 0 && this->pollfds_[i].revents & POLLHUP) {
    std::cerr << "Error: Client closed connection." << std::endl;
    removeFd(i);
    return;
  }
  std::string stringyfied_buff(buffer, rec);
  if (this->sockets_[i].hasUnfinishedRequest()) {
    this->sockets_[i].getRequest().appendBody(stringyfied_buff);
  } else {
    try {
      HTTPRequest req(stringyfied_buff, this->sockets_[i].getListenSocket(),
                      this->settings_);
      if (isCGI(req)) {
        std::cout << "Execute CGI" << std::endl;
        generateEnv(req);
        executeCGI(req, i);
        removeFd(i);
        return;
      } else {
        this->pollfds_[i].events = POLLOUT;
        this->sockets_[i].setRequest(req);
        this->sockets_[i].setState(SEND);
        std::cout << "Received from socket: " << pollfds_[i].fd << std::endl;
      }
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
  if (this->sockets_[i].getRequest().getBody().size() <
      this->sockets_[i].getRequest().getContentLength()) {
    this->sockets_[i].setUnfinishedRequest(true);
    this->pollfds_[i].events = POLLIN;
  } else {
    this->sockets_[i].setUnfinishedRequest(false);
    this->sockets_[i].setState(SEND);
    this->pollfds_[i].events = POLLOUT;
  }
  this->sockets_[i].updateTime();
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
    this->sockets_[i].setState(RECEIVE);
    this->pollfds_[i].events = POLLIN;
  }
  this->sockets_[i].updateTime();
  std::cout << "Sending to Socket: " << pollfds_[i].fd << " of size "
            << bytes_sent << std::endl;
}

void Server::newConnection(int i) {
  if (this->numfds_ > MAX_PORTS) {
    std::cout << "Error: no new Connection possible." << std::endl;
    return;
  }
  Socket new_client;
  socklen_t addrlen = sizeof(struct sockaddr);
  pollfd new_poll;
  size_t index = searchFreePoll();
  new_poll.fd = accept(this->pollfds_[i].fd,
                       (struct sockaddr*)&new_client.getAddress(), &addrlen);
  if (new_poll.fd < 0) {
    std::cout << "Error: Failed to accept connection." << std::endl;
    return;
  }
  if (fcntl(new_poll.fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
    std::cerr << "Error: fcntl." << std::endl;
    return;
  }
  new_poll.events = POLLIN;
  new_poll.revents = 0;
  new_client.setState(RECEIVE);
  new_client.setIndex(index);
  new_client.setListenSocket(this->sockets_[i].getPort());
  this->sockets_[index] = new_client;
  this->pollfds_[index] = new_poll;
  if (index == this->numfds_) {
    this->numfds_++;
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

Server::~Server() { exit(EXIT_SUCCESS); }

Server::Server(const Server& obj) { *this = obj; }

Server& Server::operator=(const Server& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

Server::Server(const Settings& settings) : settings_(settings) {
  memset(this->pollfds_, -1, MAX_PORTS);
  this->numfds_ = 0;
  for (size_t i = 0; i < this->settings_.getServers().size(); ++i) {
    if (startServer(this->settings_.getServers()[i].getName(),
                    this->settings_.getServers()[i].getPort())) {
      std::cout << "Error: failed to start Server with port "
                << this->settings_.getServers()[i].getPort() << std::endl;
    } else {
      std::cout << "Startet server on "
                << this->settings_.getServers()[i].getName() << " with Port "
                << this->settings_.getServers()[i].getPort() << std::endl;
    }
  }
}
