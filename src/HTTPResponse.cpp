#include "HTTPResponse.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <sstream>

//// Private Member Functions

void HTTPResponse::handleGET(HTTPRequest& req) {
  std::string location_path = this->settings_.getRouteRoot(
      req.getServerIndex(), req.getLocationIndex());
  std::string route_endpoint =
      this->settings_.getRouteEndpoint(req.getServerIndex(),
                                       req.getLocationIndex());
  std::cout << "URI: " << req.getURI() << std::endl;
  std::cout << "Location root: " << location_path << std::endl;
  std::cout << "Endpoint: " << route_endpoint << std::endl;
  std::string path;
  if (route_endpoint.compare(0, std::string::npos, "/") == 0) {
    path = location_path + req.getURI();
    std::cout << "i am in /" << std::endl;
  } else {
    path = req.getURI().replace(0, route_endpoint.size(), location_path);
    std::cout << "i am in replace" << std::endl;
  }
  //if (req.getURI() == "/") {
  //  path += "/index.html";
  //}
  std::cout << "path: " << path << std::endl;
  std::string mimetype =
      path.substr(path.find_last_of('.') + 1, path.size() - 1);
  std::string content_type = this->mime_types.find(mimetype)->second;
  try {
    this->setResponseLine(STATUS_200);
    this->addToHeader("Content-Type", content_type);
    this->body_ = this->createResponseBody(path, req);
    int size = this->body_.size();
    std::stringstream ss;
    ss << size;
    std::string ssize = ss.str();
    this->addToHeader("Content-Length", ssize);
  } catch (std::exception& e) {
    this->setResponseLine(STATUS_404);
    this->addToHeader("Content-Type", "text/html");
    this->body_ = this->createResponseBody(
        settings_.getServers()[0].getErrorPages()[404], req);
    this->body_.replace(this->body_.find("${URI}"), 6, req.getURI());
    int size = this->body_.size();
    std::stringstream ss;
    ss << size;
    std::string ssize = ss.str();
    this->addToHeader("Content-Length", ssize);
  }
  return;
}

void HTTPResponse::handlePOST(HTTPRequest& req) {
  std::ofstream req_file;
  std::string destination = req.getURI();
  std::string filename = destination.substr(destination.find_last_of('/') + 1,
                                            destination.size() - 1);
  req_file.open(this->settings_.getServers()[req.getServerIndex()]
                    .locations[req.getLocationIndex()]
                    .getRoot() +
                "/" + filename);
  req_file << req.getBody();
  req_file.close();
  this->setResponseLine(STATUS_201);
  this->addToHeader("Content-Length", "0");
  this->body_ = "";
}

std::string HTTPResponse::createResponseBody(std::string& path,
                                             HTTPRequest& req) {
  DIR* directory_list;
  directory_list = opendir(path.c_str());
  if (directory_list != NULL && this->settings_.getRouteAutoIndex(0, 0)) {
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
    : request_line_(obj.request_line_),
      headers_(obj.headers_),
      body_(obj.body_) {}

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& obj) {
  if (this != &obj) {
    *this = obj;
  }
  this->request_line_ = obj.request_line_;
  this->headers_ = obj.headers_;
  this->body_ = obj.body_;
  return *this;
}

HTTPResponse::HTTPResponse(HTTPRequest& req, const Settings& settings)
    : settings_(settings) {
  HTTPRequest::method req_method = req.getMethod();
  if (req.hasRequestError()) {
    this->setResponseLine(req.getRequestError());
    this->addToHeader("Content-Length", "0");
    this->body_ = "";
    return;
  }
  switch (req_method) {
    case HTTPRequest::UNKNOWN:
      this->setResponseLine(STATUS_405);
      this->addToHeader("Content-Length", "0");
      this->body_ = "";
      break;
    case HTTPRequest::OPTIONS:
      this->setResponseLine(STATUS_501);
      this->addToHeader("Content-Length", "0");
      this->body_ = "";
      break;
    case HTTPRequest::GET:
      this->handleGET(req);
      break;
    case HTTPRequest::HEAD:
      this->handleGET(req);
      this->body_ = "";
      break;
    case HTTPRequest::POST:
      this->handlePOST(req);
      break;
    case HTTPRequest::PUT:
      this->setResponseLine(STATUS_501);
      this->addToHeader("Content-Length", "0");
      this->body_ = "";
      break;
    case HTTPRequest::DELETE:
      this->setResponseLine(STATUS_501);
      this->addToHeader("Content-Length", "0");
      this->body_ = "";
      break;
    case HTTPRequest::TRACE:
      this->setResponseLine(STATUS_501);
      this->addToHeader("Content-Length", "0");
      this->body_ = "";
      break;
    case HTTPRequest::CONNECT:
      this->setResponseLine(STATUS_501);
      this->addToHeader("Content-Length", "0");
      this->body_ = "";
      break;
  }
}

//// Public Member Functions

std::string HTTPResponse::toString() const {
  std::ostringstream oss;
  oss << this->request_line_;
  for (std::map<std::string, std::string>::const_iterator it =
           this->headers_.begin();
       it != this->headers_.end(); ++it) {
    oss << it->first << ": " << it->second << "\r\n";
  }
  oss << "\r\n" << this->body_;
  return oss.str();
}

void HTTPResponse::setResponseLine(const std::string& status_code) {
  this->request_line_ = "HTTP/1.1 " + status_code + "\r\n";
}

void HTTPResponse::addToHeader(const std::string& key,
                               const std::string& value) {
  this->headers_.insert(std::pair<std::string, std::string>(key, value));
}
