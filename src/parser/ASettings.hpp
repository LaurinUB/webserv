#ifndef ASETTINGS_HPP_
#define ASETTINGS_HPP_

#include <map>
#include <string>

class ASettings { 
 public:
  ASettings() {};
  virtual ~ASettings() {};
  //std::string toString();
  std::map<std::string, std::string> settings_;

 private:
  ASettings(const ASettings& obj);
  ASettings& operator=(const ASettings& obj);
};

#endif  // ASETTINGS_HPP_
