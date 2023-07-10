#include "HTTPRequest.hpp"

#include <iostream>

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
  std::cout << "BODY" << std::endl;
  std::cout << body << std::endl;
}
