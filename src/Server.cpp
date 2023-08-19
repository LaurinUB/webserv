#include "Server.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

const int BUFFER_SIZE = 30640;

void Server::run() {
  std::cout << "\n*** Listening on ADDRESS: "
    << this->sockets_[0].getAddressString()
    << " PORT: " << this->sockets_[0].getPort() << " ***\n\n";
  while (g_signaled == 0) {
    if (poll(this->pollfds_.data(), this->pollfds_.size(), 100) == -1) {
      perror("poll");
      exitWithError("Poll failed");
    }
    checkSocketTimeout();
    this->sockets_[0].getStringState();
    std::cout << this->sockets_.size() << std::endl;
    std::cout << this->sockets_[0].getPoll().events << std::endl;
    std::cout << this->sockets_[0].getPoll().revents << std::endl;
    std::cout << this->sockets_[0].getPort() << std::endl;
    // log("====== Waiting for a new connection ======\n\n\n");
    for (size_t i = 0; i < this->pollfds_.size(); ++i) {
      // if (PRINT && this->pollfds_[i].fd != 3 && this->pollfds_[i].fd != -1) {
      //   std::cout << this->sockets_[this->pollfds_[i].fd] << "i: " << i
      //             << std::endl;
      //   std::cout << "pollfds.fd: " << this->pollfds_[i].fd << std::endl;
      // }
      if (this->pollfds_[i].fd <= 0) {
        continue;
      }
      if (this->pollfds_[i].revents & POLLIN) {
        if (this->sockets_[i].getState() == SERVER) {
          newConnection();
        } else {
          std::cout << "Recieving from Socket" << std::endl;
          handleRecieve(this->sockets_[i]);
        }
      } else if (this->pollfds_[i].revents & POLLOUT) {
        std::cout << "Sending to Socket" << std::endl;
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
  Socket listen(socket(AF_INET, SOCK_STREAM, 0));
  if (listen.getFd() < 0) {
    std::cout << "Error: can not start Socket." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Port: " << port << std::endl;
  listen.setPort(port);
  std::cout << listen.getPort() << std::endl;
  if (listen.setServerOpt() == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  if (listen.getPoll().events & POLLIN) {
    std::cout << "Event: POLLIN." << std::endl;
  }
  this->pollfds_.push_back(listen.getPoll());
  this->sockets_.push_back(listen);
  if (this->sockets_[0].getPoll().events & POLLIN) {
    std::cout << "Event: POLLIN." << std::endl;
    this->sockets_[0].getStringState();
  }
  if (this->pollfds_[0].events & POLLIN) {
    std::cout << "Event: POLLIN." << std::endl;
  }
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
  for (size_t i = 0; i < this->sockets_.size(); ++i) {
    if (this->pollfds_[i].fd == fd) {
      this->pollfds_.erase(this->pollfds_.begin() + i);
      this->sockets_.erase(this->sockets_.begin() + i);
      shutdown(fd, SHUT_RDWR);
      close(fd);
      break;
    }
  }
}

void Server::checkSocketTimeout() {
  std::vector<Socket>::iterator end = this->sockets_.end();
  for (std::vector<Socket>::iterator it = this->sockets_.begin(); it != end;
       ++it) {
    if (it->getState() != SERVER && it->checkTimeout()) {
      removeFd(it->getFd());
      this->sockets_.erase(it);
      break;
    }
  }
}

void Server::handleSend(Socket& socket) {
  sendResponse(socket);
  std::cout << "Response send" << std::endl;
  if (!socket.isKeepalive()) {
    log("Closing socket");
    removeFd(socket.getFd());
  }
}

void Server::handleRecieve(Socket& socket) {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t rec = 0;

  std::cout << "== Connected on socket: " << socket.getFd()
            << " ==" << std::endl
            << std::endl;
  rec = recv(socket.getFd(), buffer, BUFFER_SIZE, O_NONBLOCK);
  if (rec < 0) {
    exitWithError("Error: Failed to read bytes from client socket connection");
  } else if (rec == 0) {
    exitWithError("Client closed connection");
  }
  std::string stringyfied_buff(buffer);
  try {
    HTTPRequest req(stringyfied_buff);
    socket.setRequest(req);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void Server::sendResponse(Socket& socket) {
  int bytes_sent = 0;
  HTTPResponse res(socket.getRequest());
  std::string res_string = res.toString();
  bytes_sent = send(socket.getFd(), res_string.data(), res_string.size(), 0);
  if (bytes_sent < 0) {
    std::cout << "Error: sending response to client" << std::endl;
  } else if (static_cast<size_t>(bytes_sent) < res_string.size()) {
    socket.handleUnfinished(bytes_sent, res_string);
  }
  socket.updateTime();
}

void Server::newConnection() {
  if (this->pollfds_.size() > 254) {
    std::cout << "Error: no new Connection possible." << std::endl;
    return;
  }
  Socket new_client;
  socklen_t addrlen = sizeof(struct sockaddr);
  new_client.setFd(accept(this->sockets_[0].getFd(),
                          (struct sockaddr*)&new_client.getAddress(),
                          &addrlen));
  if (new_client.getFd() < 0) {
    std::cout << "Error: Failed to accept connection." << std::endl;
  }
  new_client.setState(SEND);
  this->sockets_.push_back(new_client);
  this->pollfds_.push_back(new_client.getPoll());
  std::cout << "New connection success on : " << new_client.getAddressString()
            << " with socket nbr: " << new_client.getFd() << std::endl;
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
