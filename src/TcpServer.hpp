#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#ifndef PORT
#define PORT 8080
#endif

#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "HTTPRequest.hpp"

class TcpServer {
 public:
  TcpServer(const std::string& ip_addr, int port);
  ~TcpServer();
  TcpServer(const TcpServer& obj);
  TcpServer& operator=(const TcpServer& obj);
  void startListen();

 private:
  std::string ip_addr_;
  int port_;
  int socket_;
  int new_socket_;
  struct sockaddr_in socketAddress_;
  unsigned int socketAddress_len_;

  int startServer();
  void closeServer() const;
  void acceptConnection(int& new_socket);
  std::string buildResponse(HTTPRequest& req);
  void sendResponse(HTTPRequest& req);
};

void log(const std::string& msg);
void exitWithError(const std::string& errorMsg);

#endif  // TCPSERVER_HPP_
