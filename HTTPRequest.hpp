#ifndef HTTPREQUEST_HPP_
#define HTTPREQUEST_HPP_

#include <string>
#include <map>

class HTTPRequest {
 public:
  HTTPRequest();
  ~HTTPRequest();
  HTTPRequest(const HTTPRequest& obj);
  HTTPRequest& operator=(const HTTPRequest& obj);

 private:
  std::map<std::string, std::string> header_;
  std::string body_;
};

#endif  // HTTPREQUEST_HPP_
