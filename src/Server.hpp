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
#include <vector>
#include <sstream>
#include <string>
#include <utility>

#include "HTTPRequest.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(const std::string& ip_addr, int port);
  ~Server();
  Server(const Server& obj);
  Server& operator=(const Server& obj);
  void run();

 private:
  std::string ip_addr_;
  std::vector<pollfd> pollfds_;
  std::vector<Socket> sockets_;

  int pollError(pollfd& fd);
  int startServer(int port);
  void sendResponse(int i);
  void newConnection();
  void handleRecieve(int i);
  void handleSend(int i);
  void checkSocketTimeout();
  void removeFd(int fd);
};

void log(const std::string& msg);
void exitWithError(const std::string& errorMsg);

#endif  // TCPSERVER_HPP_
