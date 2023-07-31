#include "TcpServer.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

extern sig_atomic_t g_signaled;

const int BUFFER_SIZE = 30640;
const int QUEUE_LEN = 20;

void log(const std::string& msg) { std::cout << msg << std::endl; }

void exitWithError(const std::string& errorMsg) {
  log("Error: " + errorMsg);
  exit(EXIT_FAILURE);
}

TcpServer::TcpServer(const std::string& ip_addr, int port)
    : ip_addr_(ip_addr),
      port_(port),
      listen_(),
      numfds_(1),
      socketAddress_(),
      socketAddress_len_(sizeof(socketAddress_)) {
  socketAddress_.sin_family = AF_INET;
  socketAddress_.sin_port = htons(port_);
  socketAddress_.sin_addr.s_addr = INADDR_ANY;
  if (startServer() != 0) {
    std::cout << "Failed to start server with PORT: "
              << ntohs(socketAddress_.sin_port) << std::endl;
  }
}

TcpServer::~TcpServer() {
  close(this->listen_);
  exit(EXIT_SUCCESS);
}

TcpServer::TcpServer(const TcpServer& obj) { *this = obj; }

TcpServer& TcpServer::operator=(const TcpServer& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

int TcpServer::startServer() {
  int opt = 1;
  memset(this->pollfds_, -1, 1024);
  this->listen_ = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listen_ < 0) {
    exitWithError("Cannot create socket");
    return EXIT_FAILURE;
  }
  if (setsockopt(this->listen_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1) {
    exitWithError("Cannot set socket options");
  }
  if (bind(this->listen_, (struct sockaddr*)&this->socketAddress_,
           this->socketAddress_len_) < 0) {
    exitWithError("Cannot bind to socket address");
    return EXIT_FAILURE;
  }
  if (listen(this->listen_, QUEUE_LEN) == -1) {
    exitWithError("Failed to listen on connection");
  }
  if (fcntl(this->listen_, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
    exitWithError("fcntl");
  }
  this->pollfds_[0].fd = this->listen_;
  this->pollfds_[0].events = POLLIN;

  return EXIT_SUCCESS;
}

int pollError(pollfd fd) {
  if (fd.revents & POLLERR) {
    std::cout << "Error: POLLERR" << std::endl;
    return 1;
  } else if (fd.revents & POLLPRI) {
    std::cout << "Error: POLLPRI" << std::endl;
    return 1;
  } else if (fd.revents & POLLNVAL) {
    std::cout << "Error: POLLNVAL" << std::endl;
    return 1;
  } else if (fd.revents & POLLHUP) {
    std::cout << "Error: POLLHUP" << std::endl;
    return 1;
  }
  return 0;
}

void TcpServer::run() {
  std::cout << "\n*** Listening on ADDRESS: "
            << inet_ntoa(this->socketAddress_.sin_addr)
            << " PORT: " << ntohs(this->socketAddress_.sin_port) << " ***\n\n";
  int pollres;

  while (g_signaled == 0) {
    std::cout << "Pollin'" << std::endl;
    pollres = poll(this->pollfds_, this->numfds_ + 1, 100);
    if (pollres == -1) {
      exitWithError("Poll failed");
    } else if (pollres > 0) {
      log("====== Waiting for a new connection ======\n\n\n");
    }
    std::map<int, Socket>::iterator it = getUnfinished(this->sockets_);
    if (it != this->sockets_.end()) {
      std::cout << "Send to unfinished" << std::endl;
      sendResponse(it);
    }
    for (size_t fd = 0; fd < this->numfds_; ++fd) {
      std::cout << this->pollfds_[fd].fd
                << " revents: " << this->pollfds_[fd].revents << std::endl;
      if (pollError(this->pollfds_[fd])) {
        log("Closing socket");
        this->sockets_.erase(this->sockets_.find(this->pollfds_[fd].fd));
        this->pollfds_[fd].fd = -1;
        this->numfds_--;
        continue;
      }
      if (this->pollfds_[fd].revents & POLLIN) {
        if (this->pollfds_[fd].fd == this->listen_) {
          newConnection();
          break;
        } else {
          std::cout << "Handle exsisting Connection" << std::endl;
          handleConnection(fd);
        }
      }
    }
  }
}

void TcpServer::handleConnection(size_t fd) {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t rec = 0;

  std::cout << "== Connected on socket: " << this->pollfds_[fd].fd
            << " ==" << std::endl
            << std::endl;
  rec = recv(this->pollfds_[fd].fd, buffer, BUFFER_SIZE, O_NONBLOCK);
  if (rec < 0) {
    exitWithError("Failed to read bytes from client socket connection");
  } else if (rec == 0) {
    exitWithError("Client closed connection");
  }
  std::string stringyfied_buff(buffer);
  try {
    HTTPRequest req(stringyfied_buff);
    std::cout << req << std::endl;
    this->sendResponse(req, this->pollfds_[fd].fd);
    std::cout << "Response send" << std::endl;
    if (req.getHeader().find("Connection")->second != "keep-alive") {
      log("Closing socket");
      this->sockets_.erase(this->sockets_.find(this->pollfds_[fd].fd));
      this->pollfds_[fd].fd = -1;
      this->numfds_--;
    }
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void TcpServer::sendResponse(HTTPRequest& req, int sockfd) {
  int bytesSent = 0;
  HTTPResponse res(req);
  std::string res_string = res.toString();
  bytesSent = send(sockfd, res_string.data(), res_string.size(), 0);
  if (bytesSent < 0) {
    std::cout << "Error sending response to client" << std::endl;
  } else if (static_cast<size_t>(bytesSent) < res_string.size()) {
    this->sockets_[sockfd].handleUnfinished(bytesSent, res_string);
  }
}

void TcpServer::sendResponse(std::map<int, Socket>::iterator it) {
  int bytesSent = 0;
  bytesSent = send(it->second.getFd(), it->second.getResponse().c_str(),
                   it->second.getResponseSize(), 0);
  if (bytesSent < 0) {
    std::cout << "Error sending response to client" << std::endl;
    bytesSent = 0;
  }
  if (static_cast<size_t>(bytesSent) < it->second.getResponseSize()) {
    it->second.handleUnfinished(bytesSent, it->second.getResponse());
  } else {
    it->second.setWritten(true);
  }
}

void TcpServer::newConnection() {
  pollfd new_poll;
  int new_socket =
      accept(this->listen_, (struct sockaddr*)&this->socketAddress_,
             &this->socketAddress_len_);
  if (new_socket < 0) {
    std::ostringstream ss;
    ss << "Server failed to accept incoming connection from ADDRESS: "
       << inet_ntoa(this->socketAddress_.sin_addr)
       << "; PORT: " << ntohs(this->socketAddress_.sin_port);
    exitWithError(ss.str());
  }
  std::string buff;
  new_poll.fd = new_socket;
  new_poll.events = POLLIN;
  new_poll.revents = 0;
  this->sockets_[new_socket].setPoll(new_poll);
  this->pollfds_[this->numfds_] = new_poll;
  std::cout << "New connection success on : "
            << inet_ntoa(this->socketAddress_.sin_addr)
            << " with socket nbr: " << this->pollfds_[this->numfds_].fd
            << std::endl;
  std::cout << "revents poll: " << new_poll.revents << std::endl;
  this->numfds_++;
}
