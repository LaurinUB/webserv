#ifndef GLOBALSETTINGS_HPP_
#define GLOBALSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "VServerSettings.hpp"

class GlobalSettings : public ASettings { 
 public:
  GlobalSettings() {};
  virtual ~GlobalSettings() {};
  std::vector<VServerSettings> server_settings_;

 private:
};

#endif  // GLOBALSETTINGS_HPP_
