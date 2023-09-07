#ifndef LOCATIONSETTINGS_HPP_
#define LOCATIONSETTINGS_HPP_

#include <iostream>
#include <vector>

#include "ASettings.hpp"

class LocationSettings : public ASettings {
 friend class Settings;
 public:
  LocationSettings();
  virtual ~LocationSettings();
  LocationSettings(const LocationSettings& obj);
  LocationSettings& operator=(const LocationSettings& obj);
  bool setValue(std::string key, std::string value);

  const std::vector<std::string>& getAllowedMethods() const;
  std::string getRoot() const;
  std::string getEndpoint() const;
  std::string getDefault() const;
  std::string getRedir() const;
  bool getAutoIndex() const;
  bool getAllowUpload() const;
  std::string getUploadDir() const;

 private:
  std::vector<std::string> allowed_methods_;
  std::string root_;
  std::string endpoint_;
  std::string default_;
  std::string redirect_;
  bool auto_index_;
  bool allow_upload_;
  std::string upload_dir_;
};

#endif  // LOCATIONSETTINGS_HPP_
