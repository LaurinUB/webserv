#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

#include "HTTPRequest.hpp"

#define PRINT false

enum sockState { RECEIVE, SEND, FINISHED, SERVER, UNFINISHED };

class Socket {
 public:
  Socket();
  Socket(int port);
  ~Socket();
  Socket(const Socket& obj);
  Socket& operator=(const Socket& obj);

  int getIndex() const;
  sockState getState() const;
  std::string getResponse() const;
  size_t getResponseSize() const;
  HTTPRequest& getRequest();
  void getStringState() const;
  int getPort() const;
  char* getAddressString() const;
  sockaddr_in& getAddress();
  bool isKeepalive() const;

  void setIndex(int i);
  void setKeepalive(bool state);
  void setPort(int port);
  void setState(sockState stat);
  void setRequest(HTTPRequest& req);
  void updateTime();

  bool checkTimeout();
  void handleUnfinished(int bytesSent, std::string res_string);

 private:
  struct sockaddr_in socketAddress_;
  int index_;
  time_t timestamp_;
  double timeout_;
  sockState state_;
  bool keepalive_;
  std::string response_;
  HTTPRequest request_;
};

#endif  // SOCKET_HPP_
