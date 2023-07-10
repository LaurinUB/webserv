#include "HTTPRequest.hpp"

//// Constuctors and Opearator overloads

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest& obj) {
  *this = obj;
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& obj) {
  if (this != &obj) {
    *this = obj;
  }
  return *this;
}

//// Public Member Functions
