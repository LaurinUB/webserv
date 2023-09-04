#ifndef LOCATIONSETTINGS_HPP_
#define LOCATIONSETTINGS_HPP_

#include <iostream>
#include <vector>

#include "ASettings.hpp"

class LocationSettings : public ASettings {
 public:
  LocationSettings();
  virtual ~LocationSettings();
  LocationSettings(const LocationSettings& obj);
  LocationSettings& operator=(const LocationSettings& obj);
  bool setValue(std::string key, std::string value);

  const std::vector<std::string>& getAllowedMethods() const;
  std::string getRoot() const;
  std::string getEndpoint() const;
  bool getAutoIndex() const;
  bool getAllowUpload() const;
  std::string getUploadDir() const;

 private:
  std::vector<std::string> allowed_methods_;
  std::string root_;
  std::string endpoint_;
  bool auto_index_;
  bool allow_upload_;
  std::string upload_dir_;
};

#endif  // LOCATIONSETTINGS_HPP_
