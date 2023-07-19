#include "HTTPResponse.hpp"

#include <sstream>

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
      std::cout << "GET method" << std::endl;
      break;
    case HTTPRequest::HEAD:
      std::cout << "HEAD method" << std::endl;
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
