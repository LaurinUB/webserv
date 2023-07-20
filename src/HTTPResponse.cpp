#include "HTTPResponse.hpp"

#include <sstream>

std::map<std::string, std::string> HTTPResponse::mime_types =
    HTTPResponse::getMimeTypes("./data/mime_types.csv");

std::map<std::string, std::string> HTTPResponse::getMimeTypes(
    std::string path) {
  std::ifstream fs(path);
  std::map<std::string, std::string> res;
  if (fs.is_open()) {
    std::string line;
    while (std::getline(fs, line)) {
      std::pair<std::string, std::string> parsed_pair;
      parsed_pair.second = line.substr(0, line.find(','));
      parsed_pair.first = line.substr(line.find(',') + 1, line.size() - 1);
      res.insert(parsed_pair);
    }
  }
  return res;
}

//// Constructors and Operator overloads

HTTPResponse::HTTPResponse() {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse& obj)
    : header_(obj.header_), body_(obj.body_) {}

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& obj) {
  if (this != &obj) {
    *this = obj;
  }
  this->header_ = obj.header_;
  this->body_ = obj.body_;
  return *this;
}

HTTPResponse::HTTPResponse(std::string header, std::string body)
    : header_(header), body_(body) {}

HTTPResponse::HTTPResponse(HTTPRequest& req) {
  HTTPRequest::method req_method = req.getMethod();
  switch (req_method) {
    case HTTPRequest::UNKNOWN:
      std::cout << "UNKNOWN method" << std::endl;
      break;
    case HTTPRequest::OPTIONS:
      std::cout << "OPTIONS method" << std::endl;
      break;
    case HTTPRequest::GET:
      this->handleGET(req);
      break;
    case HTTPRequest::HEAD:
      std::cout << "HEAD method" << std::endl;
      break;
    case HTTPRequest::POST:
      std::cout << "POST method" << std::endl;
      break;
    case HTTPRequest::PUT:
      std::cout << "PUT method" << std::endl;
      break;
    case HTTPRequest::DELETE:
      std::cout << "DELETE method" << std::endl;
      break;
    case HTTPRequest::TRACE:
      std::cout << "TRACE method" << std::endl;
      break;
    case HTTPRequest::CONNECT:
      std::cout << "CONNECT method" << std::endl;
      break;
  }
}

//// Public Member Functions

std::string HTTPResponse::toString() const {
  std::ostringstream oss;
  oss << this->header_ << "\r\n\r\n" << this->body_;
  return oss.str();
}

void HTTPResponse::handleGET(HTTPRequest& req) {
  std::cout << "HANDLING GET REQUEST" << std::endl;
  std::string filepath;
  if (req.getURI() == "/") {
    filepath = "/index.html";
  } else {
    filepath = req.getURI();
  }
  std::ifstream fileStream("./www" + filepath);
  if (!fileStream.is_open()) {
    std::ifstream errorPage("./data/404.html");
    std::stringstream errorPageHtml;
    errorPageHtml << errorPage.rdbuf();
    std::string errorPageString = errorPageHtml.str();
    errorPageString.replace(errorPageString.find("${URI}"), 6, req.getURI());
    this->header_ = "HTTP/1.1 404 OK\nContent-Type: text/html";
    this->body_ = errorPageString;
    return;
  }
  std::stringstream fileBuffer;
  fileBuffer << fileStream.rdbuf();
  std::string file_extension =
      filepath.substr(filepath.find_last_of('.') + 1, filepath.size() - 1);
  std::string content_type = this->mime_types.find(file_extension)->second;
  this->header_ = "HTTP/1.1 200 OK\nContent-Type: " + content_type;
  this->body_ = fileBuffer.str();
  return;
}
