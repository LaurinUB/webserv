#ifndef GLOBALSETTINGS_HPP_
#define GLOBALSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "VServerSettings.hpp"

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
  std::vector<VServerSettings> server_settings_;

 private:
};

#endif  // GLOBALSETTINGS_HPP_
