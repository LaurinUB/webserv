#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <poll.h>
#include <sys/socket.h>

#include <iostream>

class Socket {
 public:
  Socket();
  ~Socket();
  Socket(pollfd fd, bool keepalive);
  Socket(const Socket& obj);
  Socket& operator=(const Socket& obj);
  int getFd() const;
  int getREvent() const;
  pollfd getPoll() const;
  bool getState() const;
  void setOpt();
  void setState(bool state);

 private:
  pollfd pollfd_;
  bool keepalive_;
};

#endif  // SOCKET_HPP_
