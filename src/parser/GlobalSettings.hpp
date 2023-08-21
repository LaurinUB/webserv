#ifndef GLOBALSETTINGS_HPP_
#define GLOBALSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "ServerSettings.hpp"

class GlobalSettings : public ASettings {
 public:
  GlobalSettings(){};
  virtual ~GlobalSettings(){};
  GlobalSettings(const GlobalSettings& obj)
      : server_settings_(obj.server_settings_) {
    this->settings_ = obj.settings_;
  };
  GlobalSettings& operator=(GlobalSettings obj) {
    this->settings_ = obj.settings_;
    this->server_settings_ = obj.server_settings_;
    return *this;
  }
  std::vector<ServerSettings> server_settings_;

  bool setValue(std::string key, std::string value) {
    (void) key;
    (void) value;
    return true;
  };
 private:
};

#endif  // GLOBALSETTINGS_HPP_
