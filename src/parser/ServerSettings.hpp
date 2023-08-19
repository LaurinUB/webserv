#ifndef SERVERSETTINGS_HPP_
#define SERVERSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "LocationSettings.hpp"

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

 private:
  std::vector<LocationSettings> location_settings_;
};

#endif  // SERVERSETTINGS_HPP_
