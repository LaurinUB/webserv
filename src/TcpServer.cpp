#include "TcpServer.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

const int BUFFER_SIZE = 30640;
const int QUEUE_LEN = 40;

void log(const std::string& msg) { std::cout << msg << std::endl; }

void exitWithError(const std::string& errorMsg) {
  log("Error: " + errorMsg);
  exit(EXIT_FAILURE);
}

TcpServer::TcpServer(const std::string& ip_addr, int port)
    : ip_addr_(ip_addr),
      port_(port),
      listen_(),
      numfds_(1),
      socketAddress_(),
      socketAddress_len_(sizeof(socketAddress_)) {
  socketAddress_.sin_family = AF_INET;
  socketAddress_.sin_port = htons(port_);
  socketAddress_.sin_addr.s_addr = INADDR_ANY;
  if (startServer() != 0) {
    std::cout << "Error: failed to start server with PORT: "
              << ntohs(socketAddress_.sin_port) << std::endl;
  }
}

TcpServer::~TcpServer() {
  close(this->listen_);
  exit(EXIT_SUCCESS);
}

TcpServer::TcpServer(const TcpServer& obj) { *this = obj; }

TcpServer& TcpServer::operator=(const TcpServer& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

int TcpServer::startServer() {
  int opt = 1;
  memset(this->pollfds_, -1, 255);
  this->listen_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listen_ < 0) {
    exitWithError("Error: cannot create socket");
    return EXIT_FAILURE;
  }
  if (setsockopt(this->listen_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    exitWithError("Error: cannot set socket options");
  }
  if (bind(this->listen_, (struct sockaddr*)&this->socketAddress_,
           this->socketAddress_len_) < 0) {
    exitWithError("Error: cannot bind to socket address");
    return EXIT_FAILURE;
  }
  if (listen(this->listen_, QUEUE_LEN) == -1) {
    exitWithError("Error: failed to listen on connection");
  }
  if (fcntl(this->listen_, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
    exitWithError("Error: fcntl");
  }
  this->pollfds_[0].fd = this->listen_;
  this->pollfds_[0].events = POLLIN;

  return EXIT_SUCCESS;
}

int TcpServer::pollError(pollfd& poll) {
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
    this->sockets_.erase(this->sockets_.find(poll.fd));
    removeFd(poll.fd);
  }
  return res;
}

void TcpServer::handleRevents(int i) {
  if (this->pollfds_[i].fd == this->listen_) {
    newConnection();
    return;
  } else {
    std::cout << "Handle exsisting Connection" << std::endl;
    handleConnection(this->sockets_.find(this->pollfds_[i].fd)->second);
  }
}

void TcpServer::removeFd(int fd) {
  for (size_t i = 0; i < this->numfds_; ++i) {
    if (this->pollfds_[i].fd == fd) {
      this->pollfds_[i].fd = -1;
      for (size_t j = i; j < this->numfds_ - 1; ++j) {
        this->pollfds_[j] = this->pollfds_[j + 1];
      }
      this->numfds_--;
      break;
    }
  }
  if (PRINT) {
    std::cout << "close: socket size: " << this->sockets_.size() << std::endl;
    std::cout << "close: numfds_: " << this->numfds_ << std::endl;
    std::cout << "close: removeFD(" << fd << ")" << std::endl;
  }
}

void TcpServer::checkSocketTimeout() {
  int i = 0;
  std::map<int, Socket>::iterator it = this->sockets_.begin();
  while (it != this->sockets_.end()) {
    if (it->second.checkTimeout()) {
      while (this->pollfds_[i].fd != it->first) {
        i++;
      }
      this->sockets_.erase(it);
      removeFd(it->second.getFd());
      break;
    }
    it++;
  }
}

void TcpServer::run() {
  std::cout << "\n*** Listening on ADDRESS: "
            << inet_ntoa(this->socketAddress_.sin_addr)
            << " PORT: " << ntohs(this->socketAddress_.sin_port) << " ***\n\n";
  while (g_signaled == 0) {
    if (poll(this->pollfds_, this->numfds_, 100) == -1) {
      perror("poll");
      exitWithError("Poll failed");
    }
    if (PRINT) {
      log("====== Waiting for a new connection ======\n\n\n");
    }
    checkSocketTimeout();
    if (checkUnfinished(this->sockets_)) {
      continue;
    }
    for (size_t i = 0; i < this->numfds_; ++i) {
      if (this->pollfds_[i].fd <= 0) {
        continue;
      }
      if (pollError(this->pollfds_[i])) {
        break;
      }
      if (this->pollfds_[i].revents & POLLIN) {
        if (this->pollfds_[i].fd == this->listen_) {
          newConnection();
          break;
        } else {
          std::cout << "Handle exsisting Connection" << std::endl;
          handleConnection(this->sockets_.find(this->pollfds_[i].fd)->second);
        }
      }
    }
  }
}

void TcpServer::handleConnection(Socket& socket) {
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
    HTTPRequest req(stringyfied_buff, socket);
    if (PRINT) {
      std::cout << req << std::endl;
    }
    if (!req.getURI().compare(0, 9, "/cgi_bin/")){
      // handle CGI here!
    }
    this->sendResponse(req, socket);
    std::cout << "Response send" << std::endl;
    if (socket.isWritten() && !socket.isKeepalive()) {
      log("Closing socket");
      int sock = socket.getFd();
      this->sockets_.erase(this->sockets_.find(sock));
      removeFd(sock);
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void TcpServer::sendResponse(HTTPRequest& req, Socket& socket) {
  int bytesSent = 0;
  HTTPResponse res(req);
  std::string res_string = res.toString();
  bytesSent = send(socket.getFd(), res_string.data(), res_string.size(), 0);
  if (bytesSent < 0) {
    std::cout << "Error: sending response to client" << std::endl;
  } else if (static_cast<size_t>(bytesSent) < res_string.size()) {
    socket.handleUnfinished(bytesSent, res_string);
  }
  socket.updateTime();
}

void TcpServer::sendResponse(std::map<int, Socket>::iterator& it) {
  int bytesSent = 0;
  bytesSent = send(it->second.getFd(), it->second.getResponse().c_str(),
                   it->second.getResponseSize(), 0);
  if (bytesSent < 0) {
    std::cout << "Error: sending response to client" << std::endl;
    bytesSent = 0;
  }
  it->second.updateTime();
  if (static_cast<size_t>(bytesSent) < it->second.getResponseSize()) {
    it->second.handleUnfinished(bytesSent, it->second.getResponse());
  } else {
    it->second.setWritten(true);
  }
}

bool TcpServer::checkUnfinished(std::map<int, Socket>& sockets) {
  std::map<int, Socket>::iterator it = getUnfinished(sockets);
  if (it != sockets.end()) {
    sendResponse(it);
    if (!it->second.isKeepalive() && it->second.isWritten()) {
      removeFd(it->second.getFd());
      this->sockets_.erase(it);
    }
    return true;
  }
  return false;
}

void TcpServer::newConnection() {
  pollfd new_poll;
  if (this->numfds_ > 255) {
    return;
  }
  int new_socket =
      accept(this->listen_, (struct sockaddr*)&this->socketAddress_,
             &this->socketAddress_len_);
  if (new_socket < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(this->socketAddress_.sin_addr)
       << "; PORT: " << ntohs(this->socketAddress_.sin_port);
    exitWithError(ss.str());
  }
  std::string buff;
  new_poll.fd = new_socket;
  new_poll.events = POLLIN;
  new_poll.revents = 0;
  this->sockets_[new_socket].setPoll(new_poll);
  if (fcntl(new_poll.fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0) {
    exitWithError("fcntl");
  }
  std::cout << "New connection success on : "
            << inet_ntoa(this->socketAddress_.sin_addr)
            << " with socket nbr: " << this->sockets_[new_socket].getFd()
            << std::endl;
  this->pollfds_[this->numfds_] = new_poll;
  this->numfds_++;
}
