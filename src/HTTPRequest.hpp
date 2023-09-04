#ifndef HTTPREQUEST_HPP_
#define HTTPREQUEST_HPP_

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "./parser/Settings.hpp"
#include "HTTPResponseStatus.hpp"

#define MAX_CLIENT_HEADER_BUFFER 8000

class HTTPRequest {
 public:
  //// Constructors and Operator overloads
  HTTPRequest();
  ~HTTPRequest();
  HTTPRequest(const HTTPRequest& obj);
  HTTPRequest& operator=(const HTTPRequest& obj);
  HTTPRequest(std::string& input, int port, const Settings& settings);

  typedef enum {
    UNKNOWN,
    OPTIONS,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT
  } method;

  //// Accessors
  std::map<std::string, std::string>& getHeader();
  unsigned int getContentLength() const;
  std::string getBody() const;
  bool isChunked();
  void unchunkBody();
  void appendBody(std::string input);
  HTTPRequest::method getMethod() const;
  std::string getURI() const;
  std::string getQueryParam() const;
  std::string getProtocol() const;
  bool getKeepalive() const;
  bool hasRequestError() const;
  std::string getRequestError() const;
  const LocationSettings& getLocationSettings() const;
  const ServerSettings& getServerSettings() const;

 private:
  std::map<std::string, std::string> header_;
  std::string body_;
  method request_method_;
  std::string URI_;
  std::string query_param_;
  std::string protocol_version_;
  bool keepalive_;
  bool has_request_error_;
  std::string request_error_;
  ServerSettings server_settings_;
  LocationSettings location_settings_;
  //// Private Member Functions
  void removeTrailingWhitespace(std::string& str);
  method parseMethodToken(std::string& token);
  std::vector<std::string> splitLine(
      std::string line, std::vector<std::string>::value_type delim);
  std::string cleanURI(std::string& uri_str);
  void checkForErrors();
};

std::ostream& operator<<(std::ostream& os, HTTPRequest& obj);

#endif  // HTTPREQUEST_HPP_
