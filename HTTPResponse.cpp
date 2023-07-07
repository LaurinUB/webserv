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

//// Public Member Functions

std::string HTTPResponse::toString() const {
  std::ostringstream oss;
  oss << this->header_ << "\r\n\r\n" << this->body_;
  return oss.str();
}
