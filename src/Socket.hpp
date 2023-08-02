#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>

#define PRINT false

class Socket {
 public:
  Socket();
  ~Socket();
  Socket(const Socket& obj);
  Socket& operator=(const Socket& obj);

  int getFd() const;
  int getREvent() const;
  pollfd getPoll() const;
  bool isKeepalive() const;
  std::string getResponse() const;
  size_t getResponseSize() const;
  bool isWritten() const;

  void setPoll(pollfd fd);
  void setOpt();
  void setKeepalive(bool state);
  void setWritten(bool state);
  bool checkTimeout();
  void updateTime();

  void handleUnfinished(int bytesSent, std::string res_string);

 private:
  pollfd pollfd_;
  bool keepalive_;
  time_t timestamp_;
  double timeout_;
  bool data_written_;
  std::string response_;
};

std::map<int, Socket>::iterator getUnfinished(std::map<int, Socket>& sockets);

std::ostream& operator<<(std::ostream& os, const Socket& socket);

#endif  // SOCKET_HPP_
