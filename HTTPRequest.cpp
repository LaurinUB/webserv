#include "HTTPRequest.hpp"

#include <iostream>
#include <sstream>

//// Constuctors and Opearator overloads

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest& obj) { *this = obj; }

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

//// Public Member Functions

HTTPRequest::HTTPRequest(std::string& input) {
  std::string header(input.begin(),
                     input.begin() + static_cast<long>(input.find("\r\n\r\n")));
  std::string body(
      input.begin() + static_cast<long>(input.find("\r\n\r\n")) + 1,
      input.end());
  std::cout << "HEADER" << std::endl;
  std::cout << header << std::endl;
  std::istringstream iss(header);
  std::string line;
  while (std::getline(iss, line)) {
    this->request_method_ = parseMethodToken(line.substr(0, line.find(' ')));
  }
  std::cout << "BODY" << std::endl;
  std::cout << body << std::endl;
}

//// Private Member Functions

HTTPRequest::method HTTPRequest::parseMethodToken(std::string token) {
  HTTPRequest::method res;
  if (token.compare("OPTIONS") == 0) {
    res = OPTIONS;
  } else if (token.compare("GET") == 0 ) {
    res = GET;
  } else if (token.compare("HEAD") == 0 ) {
    res = HEAD;
  } else if (token.compare("POST") == 0 ) {
    res = POST;
  } else if (token.compare("PUT") == 0 ) {
    res = PUT;
  } else if (token.compare("DELETE") == 0 ) {
    res = DELETE;
  } else if (token.compare("TRACE") == 0 ) {
    res = TRACE;
  } else if (token.compare("CONNECT") == 0 ) {
    res = CONNECT;
  } else {
    throw std::runtime_error("Error: unknown request method");
  }
  return res;
}
