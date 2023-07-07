#include "TcpServer.hpp"

const int BUFFER_SIZE = 30720;

void  log(const std::string& msg) {
  std::cout << msg << std::endl;
}

void exitWithError(const std::string& errorMsg) {
  log("Error: " + errorMsg);
  exit(EXIT_FAILURE);
}

TcpServer::TcpServer(std::string ip_addr, int port)
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

TcpServer::~TcpServer() {
  closeServer();
}

TcpServer::TcpServer(const TcpServer& obj) {
*this = obj;
}

TcpServer& TcpServer::operator=(const TcpServer& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

int TcpServer::startServer() {
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    exitWithError("Cannot create socket");
    return EXIT_FAILURE;
  }
  if (bind(socket_, reinterpret_cast<sockaddr *>(&socketAddress_),
        socketAddress_len_) < 0) {
    exitWithError("Cannot connect socket to address");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void TcpServer::closeServer() {
  close(socket_);
  close(new_socket_);
  exit(EXIT_SUCCESS);
}

void  TcpServer::startListen() {
  if (listen(socket_, 20) < 0) {
    exitWithError("Socket listen failed");
  }

  std::ostringstream ss;
  ss << "\n*** Listening on ADDRESS: "
    << inet_ntoa(socketAddress_.sin_addr)
    << " PORT: " << ntohs(socketAddress_.sin_port)
    << " ***\n\n";
  log(ss.str());

  int bytesReceived;

  while (true) {
    log("====== Waiting for a new connection ======\n\n\n");
    acceptConnection(new_socket_);

    char buffer[BUFFER_SIZE] = {0};
    bytesReceived = read(new_socket_, buffer, BUFFER_SIZE);
    if (bytesReceived < 0) {
      exitWithError("Failed to read bytes from client socket connection");
    }

    std::ostringstream ss;
    ss << "------ Received Request from client ------\n\n";
    log(ss.str());

    sendResponse();

    close(new_socket_);
  }
}

void  TcpServer::acceptConnection(int& new_socket) {
  new_socket = accept(socket_, reinterpret_cast<sockaddr *>(&socketAddress_),
      &socketAddress_len_);
  if (new_socket < 0) {
    std::ostringstream ss;
    ss <<
      "Server failed to accept incoming connection from ADDRESS: "
      << inet_ntoa(socketAddress_.sin_addr) << "; PORT: "
      << ntohs(socketAddress_.sin_port);
    exitWithError(ss.str());
  }
}

std::string TcpServer::buildResponse() {
  std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body> \
    <h1> Hello Parent </h1><p> Hello from your Server :) </p></body></html>";
  std::ostringstream ss;
  ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: "
    << htmlFile.size() << "\n\n"
    << htmlFile;

  return ss.str();
}

void TcpServer::sendResponse() {
  size_t bytesSent;

  bytesSent = write(new_socket_, serverMessage_.c_str(), serverMessage_.size());

  if (bytesSent == serverMessage_.size()) {
    log("------ Server Response sent to client ------\n\n");
  } else {
    log("Error sending response to client");
  }
}
