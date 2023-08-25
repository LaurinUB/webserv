#ifndef SERVER_HPP_
#define SERVER_HPP_

#ifndef PORT
#define PORT 4040
#endif

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
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

#include "./parser/Settings.hpp"
#include "HTTPRequest.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(const std::string& ip_addr, int port);
  Server(const Settings& settings);
  ~Server();
  Server(const Server& obj);
  Server& operator=(const Server& obj);
  void run();

 private:
  std::string ip_addr_;
  size_t numfds_;
  pollfd pollfds_[250];
  std::map<int, Socket> sockets_;
  Settings settings_;

  int pollError(int i);
  int startServer(int port);
  size_t searchFreePoll();
  void sendResponse(int i);
  void newConnection();
  void handleRecieve(int i);
  void handleSend(int i);
  void checkSocketTimeout();
  void removeFd(int i);
};

void log(const std::string& msg);
void exitWithError(const std::string& errorMsg);

#endif  // TCPSERVER_HPP_
