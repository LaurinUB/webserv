#ifndef SERVER_HPP_
#define SERVER_HPP_

#ifndef PORT
#define PORT 4040
#endif

#define MAX_PORTS 250
#define BUFFER_SIZE 30640
#define QUEUE_LEN 40
#define TIMEOUT 5000
#define PYTHON "/usr/bin/pyhton"
#define BASH "/bin/bash"
#define PY ".py"
#define SH ".sh"
#define CGI_SIZE 3
#define CGI_TIMEOUT 3.0
#define PRINT false

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
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
#include <vector>

#include "./parser/Settings.hpp"
#include "HTTPRequest.hpp"
#include "Socket.hpp"

class Server {
 public:
  Server(const Settings& settings);
  ~Server();
  Server(const Server& obj);
  Server& operator=(const Server& obj);
  void run();

 private:
  std::string ip_addr_;
  size_t numfds_;
  pollfd pollfds_[MAX_PORTS];
  std::map<int, Socket> sockets_;
  std::vector<int> used_ports_;
  std::vector<std::string> used_names_;
  Settings settings_;
  char* cgi_env_[8];

  std::string replaceURIEndpoint(const HTTPRequest& req);
  int startServer(std::string ip, int port);
  int pollError(int i);
  void removeFd(int i);
  size_t searchFreePoll();
  void sendResponse(int i);
  void newConnection(int i);
  void handleReceive(int i);
  void handleSend(int i);
  void checkSocketTimeout();
  bool isCGI(const HTTPRequest& req);
  void build_cgi_response(int i, int pipfed);
  int cgi_timeout(pid_t child, int i);
  void executeCGI(const HTTPRequest& req, int i);
  void generateEnv(const HTTPRequest& req);
};

#endif  // TCPSERVER_HPP_
