#ifndef SERVERSETTINGS_HPP_
#define SERVERSETTINGS_HPP_

#include <map>
#include <string>
#include <vector>

#include "ASettings.hpp"
#include "LocationSettings.hpp"
#include "SettingsParser.hpp"

class ServerSettings : public ASettings {
 public:
  ServerSettings(){};
  virtual ~ServerSettings(){};
  ServerSettings(const ServerSettings& obj)
      : locations_(obj.locations_),
        port_(obj.port_),
        server_name_(obj.server_name_),
        error_pages_(obj.error_pages_),
        max_client_body_size_(obj.max_client_body_size_){};
  ServerSettings& operator=(const ServerSettings& obj) {
    this->locations_ = obj.locations_;
    this->port_ = obj.port_;
    this->server_name_ = obj.server_name_;
    this->error_pages_ = obj.error_pages_;
    this->max_client_body_size_ = obj.max_client_body_size_;
    return *this;
  };
  bool setValue(std::string key, std::string value) {
    value.erase(value.size() - 1);
    std::cout << "TRYING --- Key: " << key << " value: " << value << std::endl;
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
    std::cout << "SET --- Key: " << key << " value: " << value << std::endl;
    return true;
  };

  std::vector<LocationSettings> getRoutes() const { return this->locations_; }
  unsigned int getPort() const { return this->port_; };
  std::string getName() const { return this->server_name_; };
  std::map<unsigned int, std::string> getErrorPages() const {
    return this->error_pages_;
  };
  unsigned int getMaxClientBodySize() const {
    return this->max_client_body_size_;
  };

 private:
  std::vector<LocationSettings> locations_;
  unsigned int port_;
  std::string server_name_;
  std::map<unsigned int, std::string> error_pages_;
  unsigned int max_client_body_size_;
};

#endif  // SERVERSETTINGS_HPP_
