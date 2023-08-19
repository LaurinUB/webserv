#ifndef LOCATIONSETTINGS_HPP_
#define LOCATIONSETTINGS_HPP_

#include "ASettings.hpp"

class LocationSettings : public ASettings {
 public:
  LocationSettings(){};
  virtual ~LocationSettings(){};
  LocationSettings(const LocationSettings& obj) {
    this->settings_ = obj.settings_;
  };
  LocationSettings& operator=(const LocationSettings& obj) {
    this->settings_ = obj.settings_;
    return *this;
  };
};

#endif  // LOCATIONSETTINGS_HPP_
