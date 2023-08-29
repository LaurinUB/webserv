#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <dirent.h>

#include <fstream>
#include <string>

#include "./parser/Settings.hpp"
#include "HTTPRequest.hpp"

class HTTPResponse {
 public:
  //// Constructors and Operator overloads
  HTTPResponse();
  ~HTTPResponse();
  HTTPResponse(const HTTPResponse& obj);
  HTTPResponse& operator=(const HTTPResponse& obj);
  HTTPResponse(HTTPRequest& req, Settings& settings);

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

#ifndef HTTP_STATUS_CODES_
#define HTTP_STATUS_CODES_

// Information Responses
#define STATUS_100 "100 Continue"
#define STATUS_101 "101 Switching Protocols"
#define STATUS_102 "102 Processing"
#define STATUS_103 "103 Early Hints"
// Successful Responses
#define STATUS_200 "200 OK"
#define STATUS_201 "201 Created"
#define STATUS_202 "202 Accepted"
#define STATUS_203 "203 Non-Authoritative Information"
#define STATUS_204 "204 No Content"
#define STATUS_205 "205 Reset Content"
#define STATUS_206 "206 Partial Content"
#define STATUS_207 "207 Multi-Status"
#define STATUS_208 "208 Already Reported"
#define STATUS_226 "226 IM Used"
// Redirection Messages
#define STATUS_300 "300 Multiple Choices"
#define STATUS_301 "301 Moved Permanently"
#define STATUS_302 "302 Found"
#define STATUS_303 "303 See Other"
#define STATUS_304 "304 Not Modified"
#define STATUS_307 "307 Temporary Redirect"
#define STATUS_308 "308 Permanent Redirect"
// Client Error Responses
#define STATUS_400 "400 Bad Request"
#define STATUS_401 "401 Unauthorized"
#define STATUS_402 "402 Payment Required"
#define STATUS_403 "403 Forbidden"
#define STATUS_404 "404 Not Found"
#define STATUS_405 "405 Method Not Allowed"
#define STATUS_406 "406 Not Acceptable"
#define STATUS_407 "407 Proxy Authentication Required"
#define STATUS_408 "408 Request Timeout"
#define STATUS_409 "409 Conflict"
#define STATUS_410 "410 Gone"
#define STATUS_411 "411 Length Required"
#define STATUS_412 "412 Precondition Failed"
#define STATUS_413 "413 Payload Too Large"
#define STATUS_414 "414 URI Too Long"
#define STATUS_415 "415 Unsupported Media Type"
#define STATUS_416 "416 Range Not Satisfiable"
#define STATUS_417 "417 Expectation Failed"
#define STATUS_418 "418 I'm a teapot"
#define STATUS_421 "421 Misdirected Request"
#define STATUS_422 "422 Unprocessable Content"
#define STATUS_423 "423 Locked"
#define STATUS_424 "424 Failed Dependency"
#define STATUS_425 "425 Too Early"
#define STATUS_426 "426 Upgrade Required"
#define STATUS_428 "428 Precondition Required"
#define STATUS_429 "429 Too Many Requests"
#define STATUS_431 "431 Request Header Fields Too Large"
#define STATUS_451 "451 Unavailable For Legal Reasons"
// Server Error Responses
#define STATUS_500 "500 Internal Server Error"
#define STATUS_501 "501 Not Implemented"
#define STATUS_502 "502 Bad Gateway"
#define STATUS_503 "503 Service Unavailable"
#define STATUS_504 "504 Gateway Timeout"
#define STATUS_505 "505 HTTP Version Not Supported"
#define STATUS_506 "506 Variant Also Negotiates"
#define STATUS_507 "507 Insufficient Storage"
#define STATUS_508 "508 Loop Detected"
#define STATUS_510 "510 Not Extended"
#define STATUS_511 "511 Network Authentication Required"

#endif  // HTTP_STATUS_CODES_
