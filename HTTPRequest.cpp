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
  std::istringstream iss(input);
  std::string line;
  std::getline(iss, line);
  std::vector<std::string> requet_line = this->splitLine(line, ' ');
  this->request_method_ = this->parseMethodToken(*requet_line.begin());
  this->URI_ = requet_line[1];
  this->protocol_version_ = requet_line.at(2);
}

//// Private Member Functions

HTTPRequest::method HTTPRequest::parseMethodToken(std::string& token) {
  HTTPRequest::method res;
  std::remove(token.begin(), token.end(), ' ');
  std::remove(token.begin(), token.end(), '\n');
  std::remove(token.begin(), token.end(), '\r');
  if (token.compare("OPTIONS") == 0) {
    res = OPTIONS;
  } else if (token.compare("GET") == 0) {
    res = GET;
  } else if (token.compare("HEAD") == 0) {
    res = HEAD;
  } else if (token.compare("POST") == 0) {
    res = POST;
  } else if (token.compare("PUT") == 0) {
    res = PUT;
  } else if (token.compare("DELETE") == 0) {
    res = DELETE;
  } else if (token.compare("TRACE") == 0) {
    res = TRACE;
  } else if (token.compare("CONNECT") == 0) {
    res = CONNECT;
  } else {
    res = UNKNOWN;
  }
  return res;
}

std::vector<std::string> HTTPRequest::splitLine(
    std::string line, std::vector<std::string>::value_type::value_type delim) {
  std::vector<std::string> res;
  std::istringstream iss(line);
  while (!iss.eof()) {
    std::string field;
    std::getline(iss, field, delim);
    res.push_back(field);
  }
  return res;
}
