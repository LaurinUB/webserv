#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <fstream>
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

  static std::map<std::string, std::string> mime_types;
  static std::string createResponseBody(std::string& path, HTTPRequest& req);

 private:
  std::string header_;
  std::string body_;
  static std::map<std::string, std::string> getMimeTypes(std::string path);
  void handleGET(HTTPRequest& req);
};

#endif  // HTTPRESPONSE_HPP_
