#include "TcpServer.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

const int BUFFER_SIZE = 2048;
const int QUEUE_LEN = 20;

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
    std::ostringstream ss;
    ss << "Failed to start server with PORT: "
       << ntohs(socketAddress_.sin_port);
    log(ss.str());
  }
}

TcpServer::~TcpServer() { closeServer(); }

TcpServer::TcpServer(const TcpServer& obj) { *this = obj; }

TcpServer& TcpServer::operator=(const TcpServer& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

int TcpServer::startServer() {
  int opt = 1;
  memset(this->pollfds_, 0, 1024);
  this->listen_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listen_ < 0) {
    exitWithError("Cannot create socket");
    return EXIT_FAILURE;
  }
  if (setsockopt(this->listen_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    exitWithError("Cannot set socket options");
  }
  if (bind(this->listen_, (struct sockaddr*)&this->socketAddress_,
           this->socketAddress_len_) < 0) {
    exitWithError("Cannot bind to socket address");
    return EXIT_FAILURE;
  }
  if (listen(this->listen_, QUEUE_LEN) == -1) {
    exitWithError("Failed to listen on connection");
  }
  if (fcntl(this->listen_, F_SETFL, O_NONBLOCK) == -1) {
    exitWithError("fcntl");
  }
  this->pollfds_[0].fd = this->listen_;
  this->pollfds_[0].events = POLLIN;

  return EXIT_SUCCESS;
}

void TcpServer::closeServer() const {
  close(this->listen_);
  exit(EXIT_SUCCESS);
}

int pollError(pollfd fd) {
  if (fd.revents & POLLERR) {
    std::cout << "Error: POLLERR" << std::endl;
    return 1;
  } else if (fd.revents & POLLPRI) {
    std::cout << "Error: POLLPRI" << std::endl;
    return 1;
  } else if (fd.revents & POLLNVAL) {
    std::cout << "Error: POLLNVAL" << std::endl;
    return 1;
  } else if (fd.revents & POLLHUP) {
    std::cout << "Error: POLLHUP" << std::endl;
    return 1;
  }
  return 0;
}

bool TcpServer::newConnection() {
  for (size_t i = 1; i < this->numfds_; ++i) {
    if (this->pollfds_[i].revents & POLLIN) {
      return false;
    }
  }
  int new_fd;
  acceptConnection(new_fd);
  this->pollfds_[this->numfds_].fd = new_fd;
  this->pollfds_[this->numfds_].events = POLLIN;
  this->pollfds_[this->numfds_].revents = 0;
  std::cout << "New connection success on : "
            << inet_ntoa(this->socketAddress_.sin_addr)
            << " with socket nbr: " << this->pollfds_[this->numfds_].fd
            << std::endl;
  if (fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1) {
    exitWithError("fcntl");
  }
  this->numfds_++;
  return true;
}

void TcpServer::handleConnection() {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t rec = 0;

  for (size_t i = 1; i < this->numfds_; ++i) {
    if (this->pollfds_[i].revents & POLLIN) {
      std::cout << "== Connected on socket: " << this->pollfds_[i].fd
                << " ==" << std::endl
                << std::endl;
      rec = recv(this->pollfds_[i].fd, buffer, BUFFER_SIZE, O_NONBLOCK);
      if (rec < 0) {
        exitWithError("Failed to read bytes from client socket connection");
      } else if (rec == 0) {
        exitWithError("Client closed connection");
      }
      std::string stringyfied_buff(buffer);
      try {
        HTTPRequest req(stringyfied_buff);
        std::cout << req << std::endl;
        this->sendResponse(req, this->pollfds_[i].fd);
        // close(this->pollfds_[i].fd);
        // this->pollfds_[i].fd = -1;
      } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
      }
    }
  }
}

void TcpServer::run() {
  std::cout << "\n*** Listening on ADDRESS: "
            << inet_ntoa(this->socketAddress_.sin_addr)
            << " PORT: " << ntohs(this->socketAddress_.sin_port) << " ***\n\n";
  int pollres;

  while (g_signaled == 0) {
    std::cout << "Pollin'" << std::endl;
    pollres = poll(this->pollfds_, this->numfds_ + 1, 60000);
    if (pollres == -1) {
      exitWithError("Poll failed");
    } else if (pollres > 0) {
      log("====== Waiting for a new connection ======\n\n\n");
    }
    if (newConnection()) {
      continue;
    }
    handleConnection();
  }
}

void TcpServer::acceptConnection(int& new_socket) {
  new_socket = accept(this->listen_, (struct sockaddr*)&this->socketAddress_,
                      &this->socketAddress_len_);
  if (new_socket < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(this->socketAddress_.sin_addr)
       << "; PORT: " << ntohs(this->socketAddress_.sin_port);
    exitWithError(ss.str());
  }
}

void TcpServer::sendResponse(HTTPRequest& req, int sockfd) {
  ssize_t bytesSent = 0;
  HTTPResponse res(req);
  std::string res_string = res.toString();
  bytesSent = send(sockfd, res_string.c_str(), res_string.size(), 0);

  if (bytesSent == static_cast<ssize_t>(res_string.size())) {
    log("------ Server Response sent to client ------\n\n");
    std::cout << "To socket: " << sockfd << std::endl;
  } else {
    log("Error sending response to client");
  }
}
