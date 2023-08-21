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
    if (poll(this->pollfds_.data(), this->pollfds_.size(), 1000) == -1) {
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
    for (size_t i = 0; i < this->pollfds_.size(); ++i) {
      std::cout << this->sockets_[i].getPoll().events << std::endl;
      std::cout << "revents: "<< this->sockets_[i].getPoll().revents << std::endl;
      std::cout << "revents: "<< this->pollfds_[i].revents << std::endl;
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
          std::cout << this->sockets_[i + 1].getPoll().revents << std::endl;
          break;
        } else {
          handleRecieve(i);
          std::cout << "Recieving from Socket" << std::endl;
          break;
        }
      } else if (this->sockets_[i].getState() == SEND || this->pollfds_[i].revents & POLLOUT) {
        handleSend(i);
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
  this->sockets_.push_back(listen);
  this->pollfds_.push_back(listen.getPoll());
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

void Server::handleSend(int i) {
  sendResponse(i);
  std::cout << "Response send" << std::endl;
  if (!this->sockets_[i].isKeepalive()) {
    log("Closing socket");
    removeFd(this->sockets_[i].getFd());
  }
}

void Server::handleRecieve(int i) {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t rec = 0;

  std::cout << "== Connected on socket: " << this->sockets_[i].getFd()
            << " ==" << std::endl
            << std::endl;
  rec = recv(this->sockets_[i].getFd(), buffer, BUFFER_SIZE, O_NONBLOCK);
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
    this->sockets_[i].setRequest(req);
    this->sockets_[i].setState(SEND);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void Server::sendResponse(int i) {
  int bytes_sent = 0;
  HTTPResponse res(this->sockets_[i].getRequest());
  std::string res_string = res.toString();
  bytes_sent =
      send(this->sockets_[i].getFd(), res_string.data(), res_string.size(), 0);
  if (bytes_sent < 0) {
    std::cout << "Error: sending response to client" << std::endl;
  } else if (static_cast<size_t>(bytes_sent) < res_string.size()) {
    this->sockets_[i].handleUnfinished(bytes_sent, res_string);
  } else {
    this->sockets_[i].setState(RECIEVE);
  }
  this->sockets_[i].updateTime();
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
  new_client.setState(RECIEVE);
  new_client.getStringState();
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
