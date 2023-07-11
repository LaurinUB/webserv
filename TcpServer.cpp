#include "TcpServer.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

const int BUFFER_SIZE = 30720;
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

  ssize_t bytesReceived = 0;

  while (g_signaled == 0) {
    log("====== Waiting for a new connection ======\n\n\n");
    acceptConnection(this->new_socket_);

    char buffer[BUFFER_SIZE] = {0};
    bytesReceived = read(this->new_socket_, buffer, BUFFER_SIZE);
    if (bytesReceived < 0) {
      exitWithError("Failed to read bytes from client socket connection");
    }
    std::string stringyfied_buff(buffer);
    HTTPRequest req(stringyfied_buff);
    std::cout << req << std::endl;

    sendResponse();

    close(this->new_socket_);
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

void TcpServer::sendResponse() {
  ssize_t bytesSent = 0;

  bytesSent = write(this->new_socket_, this->serverMessage_.c_str(),
                    this->serverMessage_.size());

  if (bytesSent == static_cast<ssize_t>(this->serverMessage_.size())) {
    log("------ Server Response sent to client ------\n\n");
  } else {
    log("Error sending response to client");
  }
}
