#ifndef HTTPREQUEST_HPP_
#define HTTPREQUEST_HPP_

#include <iostream>
#include <map>
#include <string>
#include <vector>

class HTTPRequest {
 public:
  //// Constructors and Operator overloads
  HTTPRequest();
  ~HTTPRequest();
  HTTPRequest(const HTTPRequest& obj);
  HTTPRequest& operator=(const HTTPRequest& obj);
  HTTPRequest(std::string& input);

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
  std::map<std::string, std::string> getHeader() const;
  std::string getBody() const;
  HTTPRequest::method getMethod() const;
  std::string getURI() const;
  std::string getProtocol() const;
  bool getKeepalive() const;

 private:
  std::map<std::string, std::string> header_;
  std::string body_;
  method request_method_;
  std::string URI_;
  std::string protocol_version_;
  bool keepalive_;
  //// Private Member Functions
  void removeTrailingWhitespace(std::string& str);
  method parseMethodToken(std::string& token);
  std::vector<std::string> splitLine(
      std::string line, std::vector<std::string>::value_type delim);
  std::string cleanURI(std::string& uri_str);
};

std::ostream& operator<<(std::ostream& os, HTTPRequest& obj);

#endif  // HTTPREQUEST_HPP_
