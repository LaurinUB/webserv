#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <string>

#include "HTTPRequest.hpp"

class HTTPResponse {
 public:
  //// Constructors and Operator overloads
  HTTPResponse();
  ~HTTPResponse();
  HTTPResponse(const HTTPResponse& obj);
  HTTPResponse& operator=(const HTTPResponse& obj);
  HTTPResponse(std::string header, std::string body);
  HTTPResponse(HTTPRequest& req);

  //// Member Functions
  std::string toString() const;

 private:
  std::string header_;
  std::string body_;
};

#endif  // HTTPRESPONSE_HPP_
