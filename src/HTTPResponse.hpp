#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <sstream>
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
  HTTPResponse(const HTTPRequest& req);

  //// Member Functions
  std::string toString() const;
  void setResponseLine(const std::string& status_code);
  void addToHeader(const std::string& key, const std::string& value);

  //// Static Members
  static std::map<std::string, std::string> mime_types;
  static std::map<std::string, std::string> getMimeTypes(std::string path);

 private:
  std::string request_line_;
  std::map<std::string, std::string> headers_;
  std::string body_;
  //// Private Member Functions
  void handleGET(const HTTPRequest& req);
  void handlePOST(const HTTPRequest& req);
  void handleDELETE(const HTTPRequest& req);
  std::string createResponseBody(const std::string& path,
                                 const HTTPRequest& req);
  std::string buildDirIndexRes(DIR* directory, const HTTPRequest& req,
                               const std::string path);
  std::string buildErrorBody(const HTTPRequest& req, unsigned int status);
  std::string stringifyBodyLen() const;
};

#endif  // HTTPRESPONSE_HPP_
