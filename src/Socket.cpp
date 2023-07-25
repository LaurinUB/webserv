#include "Socket.hpp"

Socket::Socket() {}

Socket::Socket(pollfd fd, bool keepalive) : pollfd_(fd), keepalive_(keepalive) {
  this->pollfd_.events = POLLIN;
  this->pollfd_.revents = 0;
}

Socket::~Socket() {}

Socket::Socket(const Socket& obj) {
  *this = obj;
}

Socket& Socket::operator=(const Socket& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

void Socket::setOpt() {
  int opt = 1;
  if (setsockopt(this->pollfd_.fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) == -1) {
    std::cout << "Cannot set socket opt" << std::endl;
  }
}

int Socket::getFd() const {
  return this->pollfd_.fd;
}

int Socket::getREvent() const {
  return this->pollfd_.revents;
}

pollfd Socket::getPoll() const {
  return this->pollfd_;
}

bool Socket::getState() const {
  return this->keepalive_;
}

void Socket::setState(bool state) {
  this->keepalive_ = state;
}
