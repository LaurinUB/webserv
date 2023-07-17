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
      socket_(),
      new_socket_(),
      socketAddress_(),
      socketAddress_len_(sizeof(socketAddress_)),
      serverMessage_(buildResponse()) {
  socketAddress_.sin_family = AF_INET;
  socketAddress_.sin_port = htons(port_);
  socketAddress_.sin_addr.s_addr = inet_addr(ip_addr.c_str());
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
  this->socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->socket_ < 0) {
    exitWithError("Cannot create socket");
    return EXIT_FAILURE;
  }
  if (setsockopt(this->socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    exitWithError("Cannot set socket options");
  }
  if (bind(this->socket_, (struct sockaddr*)&this->socketAddress_,
           this->socketAddress_len_) < 0) {
    exitWithError("Cannot bind to socket address");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void TcpServer::closeServer() const {
  close(this->socket_);
  close(this->new_socket_);
  exit(EXIT_SUCCESS);
}

void TcpServer::startListen() {
  if (listen(this->socket_, QUEUE_LEN) < 0) {
    exitWithError("Socket listen failed");
  }

  std::ostringstream ss;
  ss << "\n*** Listening on ADDRESS: "
     << inet_ntoa(this->socketAddress_.sin_addr)
     << " PORT: " << ntohs(this->socketAddress_.sin_port) << " ***\n\n";
  log(ss.str());

  this->pollfds_[0].fd = this->socket_;
  this->pollfds_[0].events = POLLIN;
  this->pollfds_[0].revents = 0;

  ssize_t rec = 0;
  size_t  numfds = 1;
  size_t  fds = 1;

  while (g_signaled == 0) {
    if (poll(this->pollfds_, fds, 60000) == -1) {
      exitWithError("Poll failed");
    }
    log("====== Waiting for a new connection ======\n\n\n");
    for (size_t fd = 1; fd < fds + 1; ++fd) {
      if (this->pollfds_[fd].fd < 0) {
        continue;
      }
      std::cout << "fd: " << fd << std::endl;
      std::cout << "pollfds_[fd].fd = " << this->pollfds_[fd].fd << std::endl;
      if (this->pollfds_[0].revents != POLLIN) {
        // acceptConnection(this->new_socket_);
        this->pollfds_[fd].fd = this->new_socket_;
        this->pollfds_[fd].events = POLLIN;
        this->pollfds_[fd].revents = 0;
        std::cout << "New connection success on : "
          << inet_ntoa(this->socketAddress_.sin_addr) << " with socket nbr: "
          << this->pollfds_[fd].fd << std::endl;
        numfds++;
      } else {
        std::cout << "Accepting connection" << std::endl;
        acceptConnection(this->pollfds_[fd].fd);
        std::cout << "Connected on socket: " << this->pollfds_[fd].fd
          << std::endl;
        char buffer[BUFFER_SIZE] = {0};
        rec = recv(this->pollfds_[fd].fd, buffer, BUFFER_SIZE, 0);
        if (rec < 0) {
          exitWithError("Failed to read bytes from client socket connection");
        }
        std::string stringyfied_buff(buffer);
        HTTPRequest req(stringyfied_buff);
        std::cout << req << std::endl;

        sendResponse(this->pollfds_[fd].fd);
        // close(this->pollfds_[fd].fd);
      }
    }
    fds = numfds;
  }
}

void TcpServer::acceptConnection(int& new_socket) {
  new_socket = accept(this->socket_, (struct sockaddr*)&this->socketAddress_,
                      &this->socketAddress_len_);
  if (new_socket < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(this->socketAddress_.sin_addr)
       << "; PORT: " << ntohs(this->socketAddress_.sin_port);
    exitWithError(ss.str());
  }
}

std::string TcpServer::buildResponse() {
  std::ifstream htmlFile("index.html");
  std::stringstream htmlBuffer;
  htmlBuffer << htmlFile.rdbuf();
  HTTPResponse defaultRes("HTTP/1.1 200 OK\nContent-Type: text/html",
                          htmlBuffer.str());
  return defaultRes.toString();
}

void TcpServer::sendResponse(int sockfd) {
  ssize_t bytesSent = 0;

  bytesSent = send(sockfd, this->serverMessage_.c_str(),
                    this->serverMessage_.size(), O_NONBLOCK);

  if (bytesSent == static_cast<ssize_t>(this->serverMessage_.size())) {
    log("------ Server Response sent to client ------\n\n");
  } else {
    log("Error sending response to client");
  }
}
