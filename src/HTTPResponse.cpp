#include "HTTPResponse.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <sstream>


//// Private Member Functions

void HTTPResponse::handleGET(HTTPRequest& req) {
  std::string path =
      (req.getURI() == "/" ? "./www/index.html" : "./www" + req.getURI());
  std::string mimetype =
      path.substr(path.find_last_of('.') + 1, path.size() - 1);
  std::string content_type = this->mime_types.find(mimetype)->second;
  try {
    this->body_ = this->createResponseBody(path, req);
    this->header_ = "HTTP/1.1 200 OK\nContent-Type: " + content_type;
    if (req.getKeepalive()) {
      int size = this->body_.size();
      std::stringstream ss;
      ss << size;
      std::string ssize = ss.str();
      this->header_ += "\r\nContent-Length: " + ssize;
    }
  } catch (std::exception& e) {
    path = "./data/404.html";
    this->body_ = this->createResponseBody(path, req);
    this->body_.replace(this->body_.find("${URI}"), 6, req.getURI());
    this->header_ = "HTTP/1.1 404 OK\nContent-Type: text/html";
  }
  return;
}

std::string HTTPResponse::createResponseBody(std::string& path,
                                             HTTPRequest& req) {
  DIR* directory_list;
  directory_list = opendir(path.c_str());
  if (directory_list != NULL) {
    std::string res = this->buildDirIndexRes(directory_list, req, path);
    closedir(directory_list);
    return res;
  }
  std::ifstream file_stream(path);
  if (file_stream.is_open()) {
    std::stringstream file_string_stream;
    file_string_stream << file_stream.rdbuf();
    return file_string_stream.str();
  } else {
    throw std::exception();
  }
}

std::string HTTPResponse::buildDirIndexRes(DIR* directory, HTTPRequest& req,
                                           std::string path) {
  std::string res = "<html><head><title>Index of " + req.getURI() +
                    "</title></head><body><h1>Index of " + req.getURI() +
                    "</h1><hr/><pre>";
  while (true) {
    struct stat attr;
    struct dirent* test = readdir(directory);
    if (test == NULL) {
      break;
    }
    if (std::string(test->d_name) == ".") {
      continue;
    }
    stat((path + std::string(test->d_name)).c_str(), &attr);
    char time_changed[20];
    strftime(time_changed, 20, "%d-%b-%Y %H:%M", localtime(&(attr.st_ctime)));
    res += "<a href=\"" + std::string(test->d_name) + "\">" +
           std::string(test->d_name) + "</a>";
    if (std::string(test->d_name) != "..") {
      res += "\t\t\t\t\t" + std::string(time_changed) + "\t\t" +
             "file size should go here" + "\n";
    } else {
      res += "\n";
    }
  }
  res += "</pre><hr/></body></html>";
  return res;
}

//// Static Members

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
      this->handleGET(req);
      this->body_ = "";
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
