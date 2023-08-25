#include "ServerSettings.hpp"

ServerSettings::ServerSettings(){};

ServerSettings::~ServerSettings(){};

ServerSettings::ServerSettings(const ServerSettings& obj)
    : port_(obj.port_),
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
  this->error_pages_ = obj.error_pages_;
  this->max_client_body_size_ = obj.max_client_body_size_;
  return *this;
};

bool ServerSettings::setValue(std::string key, std::string value) {
  value.erase(value.size() - 1);
  if (key == "port") {
    this->port_ = std::atoi(value.c_str());
  } else if (key == "server_name") {
    this->server_name_ = value;
  } else if (key == "client_max_body_size") {
    this->max_client_body_size_ = std::atoi(value.c_str());
  } else if (std::atoi(key.c_str()) >= 200 && std::atoi(key.c_str()) < 512) {
    this->error_pages_.insert(
        std::pair<unsigned int, std::string>(std::atoi(key.c_str()), value));
  } else {
    return false;
  }
  return true;
};

std::vector<LocationSettings> ServerSettings::getRoutes() const {
  return this->locations;
}

unsigned int ServerSettings::getPort() const { return this->port_; };

std::string ServerSettings::getName() const { return this->server_name_; };

std::map<unsigned int, std::string> ServerSettings::getErrorPages() const {
  return this->error_pages_;
};

unsigned int ServerSettings::getMaxClientBodySize() const {
  return this->max_client_body_size_;
};
