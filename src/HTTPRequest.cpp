#include "HTTPRequest.hpp"

//// Accessors

std::map<std::string, std::string>& HTTPRequest::getHeader() {
  return this->header_;
}

std::string HTTPRequest::getBody() const { return this->body_; }

HTTPRequest::method HTTPRequest::getMethod() const {
  return this->request_method_;
}

std::string HTTPRequest::getURI() const { return this->URI_; }

std::string HTTPRequest::getQueryParam() const { return this->query_param_; }

std::string HTTPRequest::getProtocol() const { return this->protocol_version_; }

bool HTTPRequest::getKeepalive() const { return this->keepalive_; }

unsigned int HTTPRequest::hasRequestError() const {
  return this->has_request_error_;
}

std::string HTTPRequest::getRequestError() const {
  return this->request_error_;
}

unsigned int HTTPRequest::getContentLength() const {
  unsigned int res = 0;
  std::map<std::string, std::string>::const_iterator cl =
      this->header_.find("Content-Length");
  if (cl != this->header_.end()) {
    res = std::atoi(cl->second.c_str());
  }
  return res;
}

void HTTPRequest::appendBody(std::string input) { this->body_ += input; }

const LocationSettings& HTTPRequest::getLocationSettings() const {
  return this->location_settings_;
}

const ServerSettings& HTTPRequest::getServerSettings() const {
  return this->server_settings_;
}

//// Private Member Functions

void HTTPRequest::removeTrailingWhitespace(std::string& str) {
  int end = str.length() - 1;
  while (end >= 0 && std::isspace(str[end])) {
    end--;
  }
  str.erase(end + 1);
}

HTTPRequest::method HTTPRequest::parseMethodToken(std::string& token) {
  HTTPRequest::method res;
  std::remove(token.begin(), token.end(), ' ');
  std::remove(token.begin(), token.end(), '\n');
  std::remove(token.begin(), token.end(), '\r');
  if (token.compare("OPTIONS") == 0) {
    res = OPTIONS;
  } else if (token.compare("GET") == 0) {
    res = GET;
  } else if (token.compare("HEAD") == 0) {
    res = HEAD;
  } else if (token.compare("POST") == 0) {
    res = POST;
  } else if (token.compare("PUT") == 0) {
    res = PUT;
  } else if (token.compare("DELETE") == 0) {
    res = DELETE;
  } else if (token.compare("TRACE") == 0) {
    res = TRACE;
  } else if (token.compare("CONNECT") == 0) {
    res = CONNECT;
  } else {
    res = UNKNOWN;
  }
  return res;
}

std::vector<std::string> HTTPRequest::splitLine(
    std::string line, std::vector<std::string>::value_type delim) {
  std::vector<std::string> res;
  std::size_t current;
  std::size_t next = -1;
  do {
    current = next + 1;
    next = line.find_first_of(delim, current);
    res.push_back(line.substr(current, next - current));
  } while (next != std::vector<std::string>::value_type::npos);
  return res;
}

std::string HTTPRequest::cleanURI(std::string& uri_str) {
  std::string to_replace = "../";
  std::string::size_type n = to_replace.length();
  for (std::string::size_type i = uri_str.find(to_replace);
       i != std::string::npos; i = uri_str.find(to_replace)) {
    uri_str.erase(i, n);
  }
  // remove occurances of .. that remains
  to_replace = "..";
  n = to_replace.length();
  for (std::string::size_type i = uri_str.find(to_replace);
       i != std::string::npos; i = uri_str.find(to_replace)) {
    uri_str.erase(i, n);
  }
  return uri_str;
}

//// Constuctors and Opearator overloads

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest& obj)
    : header_(obj.header_),
      body_(obj.body_),
      request_method_(obj.request_method_),
      URI_(obj.URI_),
      query_param_(obj.query_param_),
      protocol_version_(obj.protocol_version_),
      keepalive_(obj.keepalive_),
      hostname_(obj.hostname_),
      has_request_error_(obj.has_request_error_),
      request_error_(obj.request_error_),
      server_settings_(obj.server_settings_),
      location_settings_(obj.location_settings_) {
  *this = obj;
}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& obj) {
  this->body_ = obj.body_;
  this->header_ = obj.header_;
  this->request_method_ = obj.request_method_;
  this->URI_ = obj.URI_;
  this->query_param_ = obj.query_param_;
  this->protocol_version_ = obj.protocol_version_;
  this->keepalive_ = obj.keepalive_;
  this->hostname_ = obj.hostname_;
  this->has_request_error_ = obj.has_request_error_;
  this->request_error_ = obj.request_error_;
  this->server_settings_ = obj.server_settings_;
  this->location_settings_ = obj.location_settings_;
  return *this;
}

bool HTTPRequest::isChunked() {
  std::map<std::string, std::string>::iterator it =
      this->getHeader().find("Transfer-Encoding");
  if (it != this->getHeader().end()) {
    return true;
  }
  return false;
}

void HTTPRequest::unchunkBody() {
  std::string tmp = this->getBody();
  std::string unchunked_body;
  size_t pos = 0;
  unsigned int size = 0;
  while (tmp.size()) {
    pos = tmp.find('\n');
    std::stringstream fs(tmp.substr(0, pos));
    fs >> std::hex >> size;
    if (size == 0) {
      break;
    }
    unchunked_body += tmp.substr(pos + 1, size);
    tmp = tmp.substr(pos + size);
  }
  this->body_ = unchunked_body;
}

HTTPRequest::HTTPRequest(std::string& input, int port,
                         const Settings& settings) {
  if (input.size() <= 1) {
    throw std::runtime_error("Error: tried to create request with size <= 1");
  }
  std::size_t header_end = input.find("\r\n\r\n");
  std::string header(input.begin(), input.begin() + header_end);
  std::string body(input.begin() + header_end + 4, input.end());
  std::istringstream header_iss(header);
  std::string line;
  std::getline(header_iss, line);
  std::vector<std::string> request_line = this->splitLine(line, " ");
  this->request_method_ = this->parseMethodToken(*request_line.begin());
  this->URI_ = this->cleanURI(request_line[1]);
  size_t query_param_start = this->URI_.find_first_of('?');
  if (query_param_start != std::string::npos) {
    this->query_param_ =
        this->URI_.substr(query_param_start, this->URI_.size());
    this->URI_.erase(query_param_start, this->URI_.size());
  }
  this->protocol_version_ = request_line.at(2);
  this->removeTrailingWhitespace(this->protocol_version_);
  this->has_request_error_ = false;
  while (std::getline(header_iss, line)) {
    std::vector<std::string> temp = this->splitLine(line, ": ");
    this->removeTrailingWhitespace(temp.at(2));
    this->header_.insert(
        std::pair<std::string, std::string>(temp.at(0), temp.at(2)));
  }
  if (this->header_.find("Connection")->second.compare("keep-alive") == 0) {
    this->keepalive_ = true;
  }
  if (this->header_.find("Host") != this->header_.end()) {
    this->hostname_ = this->header_.find("Host")->second;
  }
  this->server_settings_ =
      settings.getServers()[settings.matchServer(port, this->hostname_)];
  this->location_settings_ =
      this->server_settings_
          .getRoutes()[this->server_settings_.matchLocation(this->getURI())];
  this->body_ = body;
  if (isChunked()) {
    unchunkBody();
  }
  this->checkForErrors();
}

void HTTPRequest::checkForErrors() {
  std::string methods[9] = {"UNKNOWN", "OPTIONS", "GET",   "HEAD",   "POST",
                            "PUT",     "DELETE",  "TRACE", "CONNECT"};
  if (this->hasRequestError()) {
    return;
  }
  if (this->protocol_version_.compare("HTTP/1.1")) {
    this->has_request_error_ = 505;
    this->request_error_ = STATUS_505;
  } else if (this->getContentLength() < this->body_.size()) {
    this->has_request_error_ = 413;
    this->request_error_ = STATUS_413;
  } else if (this->getURI().size() + this->getQueryParam().size() >
             MAX_CLIENT_HEADER_BUFFER) {
    this->has_request_error_ = 414;
    this->request_error_ = STATUS_414;
  } else if (this->getBody().size() >
             this->server_settings_.getMaxClientBodySize()) {
    this->has_request_error_ = 413;
    this->request_error_ = STATUS_413;
  } else if (this->getMethod() == HTTPRequest::POST &&
             this->header_.find("Content-Length") == this->header_.end()) {
    this->has_request_error_ = 411;
    this->request_error_ = STATUS_411;
  } else if (this->URI_.size() < 1 || this->protocol_version_.size() < 1) {
    this->has_request_error_ = 400;
    this->request_error_ = STATUS_400;
  } else if (std::find(this->location_settings_.getAllowedMethods().begin(),
                       this->location_settings_.getAllowedMethods().end(),
                       methods[this->request_method_]) ==
             this->location_settings_.getAllowedMethods().end()) {
    this->has_request_error_ = 405;
    this->request_error_ = STATUS_405;
  } else if (!this->location_settings_.getRedir().empty() &&
             this->URI_ == this->location_settings_.getEndpoint()) {
    this->has_request_error_ = 307;
    this->request_error_ = STATUS_307;
  }
}

std::ostream& operator<<(std::ostream& os, HTTPRequest& obj) {
  std::string methods[9] = {"UNKNOWN", "OPTIONS", "GET",   "HEAD",   "POST",
                            "PUT",     "DELETE",  "TRACE", "CONNECT"};
  os << "--- Header: ---" << std::endl;
  os << "Method: " << methods[obj.getMethod()] << std::endl;
  os << "URI: " << obj.getURI() << std::endl;
  os << "Protocol: " << obj.getProtocol() << std::endl;
  os << "--- Additional Header Fields: ---" << std::endl;
  std::map<std::string, std::string> header_fields = obj.getHeader();
  for (std::map<std::string, std::string>::const_iterator it =
           header_fields.begin();
       it != header_fields.end(); ++it) {
    os << it->first << ": " << it->second << std::endl;
  }
  os << "--- Body: ---" << std::endl;
  os << obj.getBody() << std::endl;
  return os;
}
