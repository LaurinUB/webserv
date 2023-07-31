#include "Socket.hpp"

Socket::Socket() {
  std::cout << "default" << std::endl;
  this->timestamp_ = std::time(NULL);
  this->timeout_ = 1.0;
  this->keepalive_ = false;
  this->data_written_ = true;
}

Socket::~Socket() {
  std::cout << "closing Socket on: " << getFd() << std::endl;
  close(this->pollfd_.fd);
}

Socket::Socket(const Socket& obj) { *this = obj; }

Socket& Socket::operator=(const Socket& obj) {
  this->pollfd_ = obj.pollfd_;
  this->keepalive_ = obj.keepalive_;
  this->timeout_ = obj.timeout_;
  this->timestamp_ = obj.timestamp_;
  this->data_written_ = obj.data_written_;
  return *this;
}

void Socket::setOpt() {
  int opt = 1;
  if (setsockopt(this->pollfd_.fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) == -1) {
    std::cout << "Cannot set socket opt" << std::endl;
  }
}

int Socket::getFd() const { return this->pollfd_.fd; }

int Socket::getREvent() const { return this->pollfd_.revents; }

pollfd Socket::getPoll() const { return this->pollfd_; }

bool Socket::isKeepalive() const { return this->keepalive_; }

std::string Socket::getResponse() const { return this->response_; }

size_t Socket::getResponseSize() const { return this->response_.size(); }

bool Socket::isWritten() const { return this->data_written_; }

void Socket::setKeepalive(bool state) { this->keepalive_ = state; }

void Socket::setWritten(bool state) { this->data_written_ = state; }

void Socket::handleUnfinished(int bytesSent, std::string res_string) {
  this->data_written_ = false;
  this->response_ = res_string.substr(bytesSent, res_string.size());
}

void Socket::setPoll(pollfd fd) { this->pollfd_ = fd; }

bool Socket::checkTimeout() {
  time_t current = std::time(NULL);
  double time = difftime(current, this->timestamp_);
  std::cout << time << std::endl;
  if (time >= this->timeout_) {
    return true;
  }
  return false;
}

void Socket::updateTime() { this->timestamp_ = std::time(NULL); }

std::map<int, Socket>::iterator getUnfinished(std::map<int, Socket>& sockets) {
  std::map<int, Socket>::iterator it = sockets.begin();
  while (it != sockets.end()) {
    if (!it->second.isWritten()) {
      return it;
    }
    it++;
  }
  return it;
}
