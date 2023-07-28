#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#ifndef PORT
#define PORT 8080
#endif

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "HTTPRequest.hpp"

class TcpServer {
 public:
  TcpServer(const std::string& ip_addr, int port);
  ~TcpServer();
  TcpServer(const TcpServer& obj);
  TcpServer& operator=(const TcpServer& obj);
  void run();

 private:
  std::string ip_addr_;
  int port_;
  int listen_;
  size_t numfds_;
  struct sockaddr_in socketAddress_;
  unsigned int socketAddress_len_;
  std::string serverMessage_;
  pollfd pollfds_[1024];

  int startServer();
  bool acceptConnection(pollfd& fd);
  void sendResponse(HTTPRequest& req, int sockfd);
  void newConnection();
  void handleConnection(size_t fd);
};

void log(const std::string& msg);
void exitWithError(const std::string& errorMsg);

#endif  // TCPSERVER_HPP_
