#ifndef HTTPREQUEST_HPP_
#define HTTPREQUEST_HPP_

#include <map>
#include <string>
#include <vector>

class HTTPRequest {
 public:
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

 private:
  std::map<std::string, std::string> header_;
  std::string body_;
  method request_method_;
  std::string URI_;
  std::string protocol_version_;
  method parseMethodToken(std::string& token);
  std::vector<std::string> splitLine(
      std::string line, std::vector<std::string>::value_type delim);
};

#endif  // HTTPREQUEST_HPP_
