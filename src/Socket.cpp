#include "Socket.hpp"

Socket::Socket() {
  this->timestamp_ = std::time(NULL);
  this->timeout_ = 15.0;
  this->keepalive_ = false;
  this->state_ = RECEIVE;
}

Socket::Socket(int i) {
  this->timestamp_ = std::time(NULL);
  this->timeout_ = 15.0;
  this->keepalive_ = false;
  this->state_ = RECEIVE;
  this->index_ = i;
}

Socket::~Socket() {
  std::cout << "Socket: Destrucor called" << std::endl;
  // close(this->pollfd_.fd);
  // this->pollfd_.fd = -1;
}

Socket::Socket(const Socket& obj) { *this = obj; }

Socket& Socket::operator=(const Socket& obj) {
  this->index_ = obj.index_;
  this->keepalive_ = obj.keepalive_;
  this->timeout_ = obj.timeout_;
  this->timestamp_ = obj.timestamp_;
  this->socketAddress_ = obj.socketAddress_;
  this->state_ = obj.state_;
  if (!this->response_.empty())
    this->response_ = obj.response_;
  std::cout << "Assignment operator overload" << std::endl;
  return *this;
}

void Socket::setPort(int port) {
  this->socketAddress_.sin_family = AF_INET;
  this->socketAddress_.sin_port = htons(port);
  this->socketAddress_.sin_addr.s_addr = INADDR_ANY;
}

char* Socket::getAddressString() const {
  return inet_ntoa(this->socketAddress_.sin_addr);
}

sockaddr_in& Socket::getAddress() {
  return this->socketAddress_;
}

int Socket::getPort() const {
  return ntohs(this->socketAddress_.sin_port);
}

void Socket::setRequest(HTTPRequest& req) {
  this->request_ = req;
  if (req.getKeepalive()) {
    this->keepalive_ = true;
  }
}

int Socket::getIndex() const { return this->index_; }

sockState Socket::getState() const { return this->state_; }

void Socket::getStringState() const {
  switch (this->state_) {
    case RECEIVE:
      std::cout << "RECEIVE" << std::endl;
      break;
    case SEND:
      std::cout << "SEND" << std::endl;
      break;
    case FINISHED:
      std::cout << "FINISHED" << std::endl;
      break;
    case SERVER:
      std::cout << "SERVER" << std::endl;
      break;
    case UNFINISHED:
      std::cout << "UNFINISHED" << std::endl;
      break;
  }
}

bool Socket::isKeepalive() const { return this->keepalive_; }

std::string Socket::getResponse() const { return this->response_; }

size_t Socket::getResponseSize() const { return this->response_.size(); }

HTTPRequest& Socket::getRequest() { return this->request_; }

void Socket::setState(sockState state) { this->state_ = state; }
// setter Functions

void Socket::setKeepalive(bool state) { this->keepalive_ = state; }

void Socket::updateTime() { this->timestamp_ = std::time(NULL); }

// member Functions

void Socket::handleUnfinished(int bytesSent, std::string res_string) {
  this->state_ = UNFINISHED;
  this->response_ = res_string.substr(bytesSent, res_string.size());
}

void Socket::setIndex(int i) { this->index_ = i; }

bool Socket::checkTimeout() {
  time_t current = std::time(NULL);
  double time = difftime(current, this->timestamp_);
  if (time >= this->timeout_) {
    return true;
  }
  return false;
}

// public Functions

// std::ostream& operator<<(std::ostream& os, const Socket& sock) {
//   os << "Socket: " << sock.getFd() << std::endl
//     << "State: ";
//   sock.getStringState();
//   return os;
// }
