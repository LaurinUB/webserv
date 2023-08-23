#ifndef ASETTINGS_HPP_
#define ASETTINGS_HPP_

#include <map>
#include <string>

class ASettings {
 public:
  ASettings(){};
  virtual ~ASettings(){};
  std::map<std::string, std::string> settings_;

 protected:
  virtual bool setValue(std::string key, std::string value) = 0;

 private:
  ASettings(const ASettings& obj);
  ASettings& operator=(const ASettings& obj);
};

#endif  // ASETTINGS_HPP_
