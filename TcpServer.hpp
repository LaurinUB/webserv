#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#ifndef PORT
#define PORT 6969;
#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

class TcpServer {
 public:
  TcpServer(std::string ip_addr, int port);
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
  std::string serverMessage_;

  int startServer();
  void closeServer();
  void acceptConnection(int& new_socket);
  std::string buildResponse();
  void  sendResponse();
};

void  log(const std::string& msg);
void  exitWithError(const std::string& errorMsg);

#endif  // TCPSERVER_HPP_
