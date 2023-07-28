#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <dirent.h>

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

  //// Static Members
  static std::map<std::string, std::string> mime_types;
  static std::map<std::string, std::string> getMimeTypes(std::string path);

 private:
  std::string header_;
  std::string body_;
  //// Private Member Functions
  void handleGET(HTTPRequest& req);
  std::string createResponseBody(std::string& path, HTTPRequest& req);
  std::string buildDirIndexRes(DIR* directory, HTTPRequest& req,
                               std::string path);
};

#endif  // HTTPRESPONSE_HPP_
