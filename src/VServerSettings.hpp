#ifndef VSERVERSETTINGS_HPP_
#define VSERVERSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "LocationSettings.hpp"

class VServerSettings : public ASettings {
 public:
  VServerSettings() {};
  virtual ~VServerSettings() {};
  VServerSettings(const VServerSettings& obj)
      : location_settings_(obj.location_settings_){};
  VServerSettings& operator=(const VServerSettings& obj);

 private:
  std::vector<LocationSettings> location_settings_;
};

#endif  // VSERVERSETTINGS_HPP_
