#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#ifndef PORT
#define PORT 4040
#endif

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
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
#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "./parser/SettingsParser.hpp"
#include "HTTPRequest.hpp"
#include "Socket.hpp"

class TcpServer {
 public:
  TcpServer(const std::string& ip_addr, int port);
  TcpServer(const SettingsParser& settings);
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
  pollfd pollfds_[255];
  std::map<int, Socket> sockets_;

  int pollError(pollfd& fd);
  int startServer();
  void sendResponse(HTTPRequest& req, Socket& socket);
  void sendResponse(std::map<int, Socket>::iterator& it);
  void newConnection();
  void handleConnection(Socket& socket);
  bool checkUnfinished(std::map<int, Socket>& socket);
  void handleRevents(int i);
  void checkSocketTimeout();
  void removeFd(int fd);
};

void log(const std::string& msg);
void exitWithError(const std::string& errorMsg);

#endif  // TCPSERVER_HPP_
