#include "HTTPResponse.hpp"

//// Private Member Functions

void HTTPResponse::handleGET(const HTTPRequest& req) {
  std::string location_path = req.getLocationSettings().getRoot();
  std::string route_endpoint = req.getLocationSettings().getEndpoint();
  std::string path;
  if (route_endpoint.compare(0, std::string::npos, "/") == 0) {
    path = location_path + req.getURI();
  } else {
    path = req.getURI().replace(0, route_endpoint.size(), location_path);
  }
  try {
    this->setResponseLine(STATUS_200);
    this->body_ = this->createResponseBody(path, req);
    this->addToHeader("Content-Length", this->stringifyBodyLen());
  } catch (std::exception& e) {
    this->setResponseLine(STATUS_404);
    this->addToHeader("Content-Type", "text/html");
    this->body_ = this->buildErrorBody(req, 404);
    this->addToHeader("Content-Length", this->stringifyBodyLen());
  }
  return;
}

void HTTPResponse::handlePOST(const HTTPRequest& req) {
  std::ofstream req_file;
  std::string destination = req.getURI();
  std::string filename = destination.substr(destination.find_last_of('/') + 1,
                                            destination.size() - 1);
  req_file.open(req.getLocationSettings().getRoot() + "/" + filename);
  req_file << req.getBody();
  req_file.close();
  this->setResponseLine(STATUS_201);
  this->addToHeader("Content-Length", "0");
  this->body_ = "";
}

void HTTPResponse::handleDELETE(const HTTPRequest& req) {
  std::string destination = req.getURI();
  std::string filename = destination.substr(destination.find_last_of('/') + 1,
                                            destination.size() - 1);
  std::string absolute_path =
      req.getLocationSettings().getRoot() + "/" + filename;
  if (remove(absolute_path.c_str()) != 0) {
    this->setResponseLine(STATUS_204);
    this->addToHeader("Content-Length", "0");
    this->body_ = "";
  } else {
    this->setResponseLine(STATUS_200);
    this->addToHeader("Content-Length", "0");
    this->body_ = "";
  }
  return;
}

std::string HTTPResponse::createResponseBody(const std::string& path,
                                             const HTTPRequest& req) {
  std::string URIpath = path;
  DIR* directory_list;
  directory_list = opendir(path.c_str());
  if (directory_list != NULL && req.getLocationSettings().getAutoIndex()) {
    std::string res = this->buildDirIndexRes(directory_list, req, path);
    closedir(directory_list);
    return res;
  } else if (directory_list != NULL &&
             !req.getLocationSettings().getAutoIndex()) {
    URIpath += '/' + req.getLocationSettings().getDefault();
    closedir(directory_list);
  }
  std::string mimetype =
      URIpath.substr(URIpath.find_last_of('.') + 1, URIpath.size() - 1);
  std::string content_type;
  std::map<std::string, std::string>::iterator content_type_res =
      this->mime_types.find(mimetype);
  if (content_type_res == this->mime_types.end()) {
    content_type = "application/octet-stream";
  } else {
    content_type = content_type_res->second;
  }
  this->addToHeader("Content-Type", content_type);
  std::ifstream fs(URIpath);
  if (fs.is_open()) {
    std::stringstream fss;
    fss << fs.rdbuf();
    return fss.str();
  } else {
    throw std::exception();
  }
}

std::string HTTPResponse::buildDirIndexRes(DIR* directory,
                                           const HTTPRequest& req,
                                           const std::string path) {
  std::string res = "<html><head><title>Index of " + req.getURI() +
                    "</title></head><body><h1>Index of " + req.getURI() +
                    "</h1><hr/><pre>";
  while (true) {
    struct stat attr;
    struct dirent* test = readdir(directory);
    if (test == NULL) {
      break;
    }
    std::ifstream file((path + '/' + test->d_name).c_str(),
                       std::ios::in | std::ios::binary);
    file.seekg(0, std::ios::end);
    int file_size = file.tellg();
    file.close();
    if (std::string(test->d_name) == ".") {
      continue;
    }
    stat((path + std::string(test->d_name)).c_str(), &attr);
    char time_changed[20];
    strftime(time_changed, 20, "%d-%b-%Y %H:%M", localtime(&(attr.st_ctime)));
    res += "<a href=\"" + req.getURI() + '/' + std::string(test->d_name) +
           "\">" + std::string(test->d_name) + "</a>";
    if (std::string(test->d_name) != "..") {
      res += "\t\t\t\t\t" + std::string(time_changed) + "\t\t" +
             std::to_string(file_size) + "\n";
    } else {
      res += "\n";
    }
  }
  res += "</pre><hr/></body></html>";
  while (res.find("//") != std::string::npos) {
    res = res.erase(res.find("//"), 1);
  }
  return res;
}

std::string HTTPResponse::buildErrorBody(const HTTPRequest& req,
                                         unsigned int status) {
  std::string response;
  if (req.getServerSettings().getErrorPages().find(status) !=
          req.getServerSettings().getErrorPages().end() &&
      access(req.getServerSettings().getErrorPages()[status].c_str(), F_OK) ==
          0) {
    response = this->createResponseBody(
        req.getServerSettings().getErrorPages()[status], req);
  } else {
    response = "Error " + req.getRequestError();
  }
  if (response.find("${URI}") != std::string::npos) {
    response.replace(response.find("${URI}"), 6, req.getURI());
  }
  return response;
}

std::string HTTPResponse::stringifyBodyLen() const {
  int size = this->body_.size();
  std::stringstream ss;
  ss << size;
  return ss.str();
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

HTTPResponse::HTTPResponse(const HTTPRequest& req) {
  HTTPRequest::method req_method = req.getMethod();
  if (req.hasRequestError()) {
    this->setResponseLine(req.getRequestError());
    this->body_ = this->buildErrorBody(req, req.hasRequestError());
    this->addToHeader("Content-Length", this->stringifyBodyLen());
    if (req.getRequestError() == STATUS_307) {
      this->addToHeader("Location", req.getLocationSettings().getRedir());
    }
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
      this->handleDELETE(req);
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
