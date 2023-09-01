#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <dirent.h>

#include <fstream>
#include <string>

#include "./parser/Settings.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponseStatus.hpp"

class HTTPResponse {
 public:
  //// Constructors and Operator overloads
  HTTPResponse();
  ~HTTPResponse();
  HTTPResponse(const HTTPResponse& obj);
  HTTPResponse& operator=(const HTTPResponse& obj);
  HTTPResponse(HTTPRequest& req, const Settings& settings);

  //// Member Functions
  std::string toString() const;
  void setResponseLine(const std::string& status_code);
  void addToHeader(const std::string& key, const std::string& value);

  //// Static Members
  static std::map<std::string, std::string> mime_types;
  static std::map<std::string, std::string> getMimeTypes(std::string path);

 private:
  Settings settings_;
  std::string request_line_;
  std::map<std::string, std::string> headers_;
  std::string body_;
  //// Private Member Functions
  void handleGET(HTTPRequest& req);
  void handlePOST(HTTPRequest& req);
  std::string createResponseBody(std::string& path, HTTPRequest& req);
  std::string buildDirIndexRes(DIR* directory, HTTPRequest& req,
                               std::string path);
};

#endif  // HTTPRESPONSE_HPP_
