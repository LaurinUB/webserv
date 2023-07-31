#include "HTTPRequest.hpp"

#include <iostream>
#include <sstream>

//// Accessors

std::map<std::string, std::string> HTTPRequest::getHeader() const {
  return this->header_;
}

std::string HTTPRequest::getBody() const { return this->body_; }

HTTPRequest::method HTTPRequest::getMethod() const {
  return this->request_method_;
}

std::string HTTPRequest::getURI() const { return this->URI_; }

std::string HTTPRequest::getProtocol() const { return this->protocol_version_; }

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
    std::string line, std::vector<std::string>::value_type delim) {
  std::vector<std::string> res;
  std::size_t current;
  std::size_t next = -1;
  do {
    current = next + 1;
    next = line.find_first_of(delim, current);
    res.push_back(line.substr(current, next - current));
  } while (next != std::vector<std::string>::value_type::npos);
  return res;
}

std::string HTTPRequest::cleanURI(std::string& uri_str) {
  std::string to_replace = "../";
  std::string::size_type n = to_replace.length();
  for (std::string::size_type i = uri_str.find(to_replace);
       i != std::string::npos; i = uri_str.find(to_replace)) {
    uri_str.erase(i, n);
  }
  // remove occurances of .. that remains
  to_replace = "..";
  n = to_replace.length();
  for (std::string::size_type i = uri_str.find(to_replace);
       i != std::string::npos; i = uri_str.find(to_replace)) {
    uri_str.erase(i, n);
  }
  return uri_str;
}

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

HTTPRequest::HTTPRequest(std::string& input) {
  if (input.size() <= 1) {
    throw std::runtime_error("Error: tried to create request with size <= 1");
  }
  std::size_t header_end = input.find("\r\n\r\n");
  std::string header(input.begin(), input.begin() + header_end);
  std::string body(input.begin() + header_end + 4, input.end());
  std::istringstream header_iss(header);
  std::string line;
  std::getline(header_iss, line);
  std::vector<std::string> request_line = this->splitLine(line, " ");
  this->request_method_ = this->parseMethodToken(*request_line.begin());
  this->URI_ = this->cleanURI(request_line[1]);
  this->protocol_version_ = request_line.at(2);
  while (std::getline(header_iss, line)) {
    std::vector<std::string> temp = this->splitLine(line, ": ");
    temp.at(2).pop_back();
    this->header_.insert(
        std::pair<std::string, std::string>(temp.at(0), temp.at(2)));
  }
  this->body_ = body;
}

std::ostream& operator<<(std::ostream& os, HTTPRequest& obj) {
  std::string methods[9] = {"UNKNOWN", "OPTIONS", "GET",   "HEAD",   "POST",
                            "PUT",     "DELETE",  "TRACE", "CONNECT"};
  os << "--- Header: ---" << std::endl;
  os << "Method: " << methods[obj.getMethod()] << std::endl;
  os << "URI: " << obj.getURI() << std::endl;
  os << "Protocol: " << obj.getProtocol() << std::endl;
  os << "--- Additional Header Fields: ---" << std::endl;
  std::map<std::string, std::string> header_fields = obj.getHeader();
  for (std::map<std::string, std::string>::const_iterator it =
           header_fields.begin();
       it != header_fields.end(); ++it) {
    os << it->first << ": " << it->second << std::endl;
  }
  os << "--- Body: ---" << std::endl;
  os << obj.getBody() << std::endl;
  return os;
}
