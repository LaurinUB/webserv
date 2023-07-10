#ifndef HTTPREQUEST_HPP_
#define HTTPREQUEST_HPP_

#include <map>
#include <string>

class HTTPRequest {
 public:
  HTTPRequest();
  ~HTTPRequest();
  HTTPRequest(const HTTPRequest& obj);
  HTTPRequest& operator=(const HTTPRequest& obj);
  HTTPRequest(std::string& input);

  typedef enum { OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT } method;

  typedef enum {
    HTTP1_1,
  } version;

 private:
  std::map<std::string, std::string> header_;
  std::string body_;
  method request_method_;
  // std::string URI_;
  // version protocol_version_;
  method parseMethodToken(std::string token);
};

#endif  // HTTPREQUEST_HPP_
