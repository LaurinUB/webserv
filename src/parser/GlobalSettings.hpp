#ifndef GLOBALSETTINGS_HPP_
#define GLOBALSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "ServerSettings.hpp"

class GlobalSettings : public ASettings {
 public:
  GlobalSettings(){};
  virtual ~GlobalSettings(){};
  GlobalSettings(const GlobalSettings& obj) : servers(obj.servers) {
    this->settings_ = obj.settings_;
  };
  GlobalSettings& operator=(GlobalSettings obj) {
    this->settings_ = obj.settings_;
    this->servers = obj.servers;
    return *this;
  }
  std::vector<ServerSettings> servers;

  bool setValue(std::string key, std::string value) {
    (void)key;
    (void)value;
    return true;
  };
  const std::vector<ServerSettings> getServers() const {
    return this->servers;
  };

 private:
};

#endif  // GLOBALSETTINGS_HPP_
