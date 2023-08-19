#include "Socket.hpp"

const int QUEUE_LEN = 40;

Socket::Socket() {
  this->timestamp_ = std::time(NULL);
  this->timeout_ = 15.0;
  this->keepalive_ = false;
  this->state_ = RECIEVE;
}

Socket::Socket(int fd) {
  this->timestamp_ = std::time(NULL);
  this->timeout_ = 15.0;
  this->keepalive_ = false;
  this->state_ = RECIEVE;
  this->pollfd_.fd = fd;
}

Socket::~Socket() {
  std::cout << "Socket: Destrucor called" << std::endl;
  // close(this->pollfd_.fd);
  // this->pollfd_.fd = -1;
}

Socket::Socket(const Socket& obj) { *this = obj; }

Socket& Socket::operator=(const Socket& obj) {
  this->pollfd_ = obj.pollfd_;
  this->keepalive_ = obj.keepalive_;
  this->timeout_ = obj.timeout_;
  this->timestamp_ = obj.timestamp_;
  this->socketAddress_ = obj.socketAddress_;
  this->state_ = obj.state_;
  return *this;
}

void Socket::setPort(int port) {
  this->socketAddress_.sin_family = AF_INET;
  this->socketAddress_.sin_port = port;
  this->socketAddress_.sin_addr.s_addr = INADDR_ANY;
}

char* Socket::getAddressString() const {
  return inet_ntoa(this->socketAddress_.sin_addr);
}

sockaddr_in& Socket::getAddress() {
  return this->socketAddress_;
}

int Socket::getPort() const {
  return this->socketAddress_.sin_port;
}

void Socket::setRequest(HTTPRequest req) {
  this->request_ = req;
  if (req.getKeepalive()) {
    this->keepalive_ = true;
  }
}

int Socket::setServerOpt() {
  int opt = 1;
  socklen_t addrlen = sizeof(this->socketAddress_);
  if (setsockopt(this->pollfd_.fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) == -1) {
    std::cerr << "Error: cannot set socket opt." << std::endl;
  }
  std::cout << "Success: Socket opt set." << std::endl;
  if (bind(this->pollfd_.fd, (struct sockaddr*)&this->socketAddress_,
        addrlen)) {
    std::cerr << "Error: cannot bind to address." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Success: Socket bind." << std::endl;
  if (listen(this->pollfd_.fd, QUEUE_LEN) == -1) {
    std::cerr << "Error: failed to listen on connection" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Success: Socket listen." << std::endl;
  if (fcntl(this->pollfd_.fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
    std::cerr << "Error: fcntl" <<std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Success: Socket fcntl." << std::endl;
  this->pollfd_.events = POLLIN;
  this->state_ = SERVER;
  return EXIT_SUCCESS;
}

int Socket::getFd() const { return this->pollfd_.fd; }

sockState Socket::getState() const { return this->state_; }

void Socket::getStringState() const {
  switch (this->state_) {
    case RECIEVE:
      std::cout << "RECIEVE" << std::endl;
    case SEND:
      std::cout << "SEND" << std::endl;
    case FINISHED:
      std::cout << "FINISHED" << std::endl;
    case SERVER:
      std::cout << "SERVER" << std::endl;
  }
}

pollfd Socket::getPoll() const { return this->pollfd_; }

bool Socket::isKeepalive() const { return this->keepalive_; }

std::string Socket::getResponse() const { return this->response_; }

size_t Socket::getResponseSize() const { return this->response_.size(); }

HTTPRequest& Socket::getRequest() { return this->request_; }

void Socket::setState(sockState state) {
  this->state_ = state;
}

void Socket::setKeepalive(bool state) { this->keepalive_ = state; }

void Socket::handleUnfinished(int bytesSent, std::string res_string) {
  this->state_ = SEND;
  this->pollfd_.events = POLLOUT;
  this->response_ = res_string.substr(bytesSent, res_string.size());
}

void Socket::setFd(int fd) { this->pollfd_.fd = fd; }

void Socket::setPoll(pollfd fd) { this->pollfd_ = fd; }

bool Socket::checkTimeout() {
  time_t current = std::time(NULL);
  double time = difftime(current, this->timestamp_);
  if (time >= this->timeout_) {
    return true;
  }
  return false;
}

void Socket::updateTime() { this->timestamp_ = std::time(NULL); }

std::ostream& operator<<(std::ostream& os, const Socket& sock) {
  os << "Socket: " << sock.getFd() << std::endl
    << "State: ";
  sock.getStringState();
  return os;
}
