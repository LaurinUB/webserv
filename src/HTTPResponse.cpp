#include "HTTPResponse.hpp"

#include <sstream>

std::map<std::string, std::string> HTTPResponse::mime_types =
    HTTPResponse::getMimeTypes("./data/mime_types.csv");

std::map<std::string, std::string> HTTPResponse::getMimeTypes(
    std::string path) {
  std::ifstream fs(path);
  std::map<std::string, std::string> res;
  if (fs.is_open()) {
    std::string line;
    while (std::getline(fs, line)) {
      std::pair<std::string, std::string> parsed_pair;
      parsed_pair.second = line.substr(0, line.find(','));
      parsed_pair.first = line.substr(line.find(',') + 1, line.size() - 1);
      res.insert(parsed_pair);
    }
  }
  return res;
}

std::string HTTPResponse::createResponseBody(std::string& path) {
  std::ifstream file_stream(path);
  if (file_stream.is_open()) {
    std::stringstream file_string_stream;
    file_string_stream << file_stream.rdbuf();
    std::cout << file_string_stream.str().size() << std::endl;
    file_stream.close();
    return file_string_stream.str();
  } else {
    throw std::exception();
  }
}

//// Constructors and Operator overloads

HTTPResponse::HTTPResponse() {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse& obj)
    : header_(obj.header_), body_(obj.body_) {}

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& obj) {
  if (this != &obj) {
    *this = obj;
  }
  this->header_ = obj.header_;
  this->body_ = obj.body_;
  return *this;
}

HTTPResponse::HTTPResponse(std::string header, std::string body)
    : header_(header), body_(body) {}

HTTPResponse::HTTPResponse(HTTPRequest& req) {
  HTTPRequest::method req_method = req.getMethod();
  switch (req_method) {
    case HTTPRequest::UNKNOWN:
      std::cout << "UNKNOWN method" << std::endl;
      break;
    case HTTPRequest::OPTIONS:
      std::cout << "OPTIONS method" << std::endl;
      break;
    case HTTPRequest::GET:
      this->handleGET(req);
      break;
    case HTTPRequest::HEAD:
      std::cout << "HEAD method" << std::endl;
      this->handleGET(req);
      this->body_ = "";
      break;
    case HTTPRequest::POST:
      std::cout << "POST method" << std::endl;
      break;
    case HTTPRequest::PUT:
      std::cout << "PUT method" << std::endl;
      break;
    case HTTPRequest::DELETE:
      std::cout << "DELETE method" << std::endl;
      break;
    case HTTPRequest::TRACE:
      std::cout << "TRACE method" << std::endl;
      break;
    case HTTPRequest::CONNECT:
      std::cout << "CONNECT method" << std::endl;
      break;
  }
}

//// Public Member Functions

std::string HTTPResponse::toString() const {
  std::ostringstream oss;
  oss << this->header_ << "\r\n\r\n" << this->body_;
  return oss.str();
}

void HTTPResponse::handleGET(HTTPRequest& req) {
  std::string path =
      (req.getURI() == "/" ? "./www/index.html" : "./www" + req.getURI());
  std::string mimetype =
      path.substr(path.find_last_of('.') + 1, path.size() - 1);
  std::string content_type = this->mime_types.find(mimetype)->second;
  try {
    this->body_ = this->createResponseBody(path);
    this->header_ = "HTTP/1.1 200 OK\nContent-Type: " + content_type;
    if (req.getHeader().find("Connection")->second.compare("keep-alive") == 0) {
      int size = this->body_.size();
      std::stringstream ss;
      ss << size;
      std::string ssize = ss.str();
      this->header_ += "\r\nContent-Length: " + ssize;
    }
  } catch (std::exception& e) {
    path = "./data/404.html";
    this->body_ = this->createResponseBody(path);
    this->body_.replace(this->body_.find("${URI}"), 6, req.getURI());
    this->header_ = "HTTP/1.1 404 OK\nContent-Type: text/html";
  }
  return;
}
