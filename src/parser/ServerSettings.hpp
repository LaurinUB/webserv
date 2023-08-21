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
      : location_settings_(obj.location_settings_){};
  ServerSettings& operator=(const ServerSettings& obj) {
    this->settings_ = obj.settings_;
    this->location_settings_ = obj.location_settings_;
    return *this;
  };
  bool setValue(std::string key, std::string value) {
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

 private:
  std::vector<LocationSettings> location_settings_;
  unsigned int port_;
  std::string server_name_;
  std::map<unsigned int, std::string> error_pages_;
  unsigned int max_client_body_size_;
};

#endif  // SERVERSETTINGS_HPP_
