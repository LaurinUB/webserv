#ifndef LOCATIONSETTINGS_HPP_
#define LOCATIONSETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"

class LocationSettings : public ASettings {
 public:
  LocationSettings(){};
  virtual ~LocationSettings(){};
  LocationSettings(const LocationSettings& obj)
      : allowed_methods_(obj.allowed_methods_),
        root_(obj.root_),
        auto_index_(obj.auto_index_),
        allow_upload_(obj.allow_upload_),
        upload_dir_(obj.upload_dir_){};
  LocationSettings& operator=(const LocationSettings& obj) {
    this->allowed_methods_ = obj.allowed_methods_;
    this->root_ = obj.root_;
    this->auto_index_ = obj.auto_index_;
    this->allow_upload_ = obj.allow_upload_;
    this->upload_dir_ = obj.upload_dir_;
    return *this;
  };
  bool setValue(std::string key, std::string value) {
    if (key == "root") {
      this->root_ = value;
    } else if (key == "auto-index") {
      this->auto_index_ = value == "true" ? true : false;
    } else if (key == "allow-method") {
      this->allowed_methods_.push_back(value);
    } else if (key == "allow-upload") {
      this->allow_upload_ = value == "true" ? true : false;
    } else if (key == "upload-directory") {
      this->upload_dir_ = value;
    } else {
      return false;
    }
    return true;
  };

  std::vector<std::string> getAllowedMethods() const {
    return this->allowed_methods_;
  };
  std::string getRoot() const { return this->root_; };
  bool getAutoIndex() const { return this->auto_index_; };
  bool getAllowUpload() const { return this->allow_upload_; };
  std::string getUploadDir() const { return this->upload_dir_; };

 private:
  std::vector<std::string> allowed_methods_;
  std::string root_;
  bool auto_index_;
  bool allow_upload_;
  std::string upload_dir_;
};

#endif  // LOCATIONSETTINGS_HPP_
