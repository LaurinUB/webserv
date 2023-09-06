#include "ServerSettings.hpp"

ServerSettings::ServerSettings() {}

ServerSettings::~ServerSettings() {}

ServerSettings::ServerSettings(const ServerSettings& obj)
    : port_(obj.port_),
      listen_(obj.listen_),
      server_name_(obj.server_name_),
      error_pages_(obj.error_pages_),
      max_client_body_size_(obj.max_client_body_size_) {
  for (std::vector<LocationSettings>::size_type i = 0; i < obj.locations.size();
       ++i) {
    this->locations.push_back(obj.locations[i]);
  }
}

ServerSettings& ServerSettings::operator=(const ServerSettings& obj) {
  for (std::vector<LocationSettings>::size_type i = 0; i < obj.locations.size();
       ++i) {
    this->locations.push_back(obj.locations[i]);
  }
  this->port_ = obj.port_;
  this->server_name_ = obj.server_name_;
  this->listen_ = obj.listen_;
  this->error_pages_ = obj.error_pages_;
  this->max_client_body_size_ = obj.max_client_body_size_;
  return *this;
}

bool ServerSettings::setValue(std::string key, std::string value) {
  value.erase(value.size() - 1);
  if (key == "port") {
    this->port_ = std::atoi(value.c_str());
  } else if (key == "server_name") {
    this->server_name_ = value;
  } else if (key == "listen") {
    this->listen_ = value;
  } else if (key == "client_max_body_size") {
    this->max_client_body_size_ = std::atoi(value.c_str());
  } else if (std::atoi(key.c_str()) >= 200 && std::atoi(key.c_str()) < 512) {
    this->error_pages_.insert(
        std::pair<unsigned int, std::string>(std::atoi(key.c_str()), value));
  } else {
    return false;
  }
  return true;
}

std::vector<LocationSettings> ServerSettings::getRoutes() const {
  return this->locations;
}

unsigned int ServerSettings::getPort() const { return this->port_; }

std::string ServerSettings::getName() const { return this->server_name_; }

std::string ServerSettings::getListen() const { return this->listen_; }

std::map<unsigned int, std::string> ServerSettings::getErrorPages() const {
  return this->error_pages_;
}

unsigned int ServerSettings::getMaxClientBodySize() const {
  return this->max_client_body_size_;
}

unsigned int ServerSettings::matchLocation(const std::string& endpoint) const {
  unsigned int res = 0;
  for (size_t i = 0; i < this->locations.size(); ++i) {
    size_t found_at = endpoint.find(this->locations[i].getEndpoint());
    if (found_at == 0 && this->locations[i].getEndpoint().size() > 1) {
      return res;
    }
    ++res;
  }
  return 0;
}
