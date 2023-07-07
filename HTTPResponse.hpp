#ifndef HTTPRESPONSE_HPP_
#define HTTPRESPONSE_HPP_

#include <string>

class HTTPResponse {
 public:
  //// Constructors and Operator overloads
  HTTPResponse();
  ~HTTPResponse();
  HTTPResponse(const HTTPResponse& obj);
  HTTPResponse& operator=(const HTTPResponse& obj);
  HTTPResponse(std::string header, std::string body);

  //// Member Functions
  std::string toString() const;

 private:
  std::string header_;
  std::string body_;
};

#endif  // HTTPRESPONSE_HPP_
