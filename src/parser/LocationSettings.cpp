#include "LocationSettings.hpp"

LocationSettings::LocationSettings()
    : root_("./www/"),
      endpoint_("/"),
      default_("index.html"),
      auto_index_(false),
      allow_upload_(false),
      upload_dir_("./www/uploads") {}

LocationSettings::~LocationSettings() {}

LocationSettings::LocationSettings(const LocationSettings& obj)
    : allowed_methods_(obj.allowed_methods_),
      root_(obj.root_),
      endpoint_(obj.endpoint_),
      default_(obj.default_),
      redirect_(obj.redirect_),
      auto_index_(obj.auto_index_),
      allow_upload_(obj.allow_upload_),
      upload_dir_(obj.upload_dir_) {}

LocationSettings& LocationSettings::operator=(const LocationSettings& obj) {
  this->allowed_methods_ = obj.allowed_methods_;
  this->root_ = obj.root_;
  this->endpoint_ = obj.endpoint_;
  this->default_ = obj.default_;
  this->auto_index_ = obj.auto_index_;
  this->allow_upload_ = obj.allow_upload_;
  this->upload_dir_ = obj.upload_dir_;
  this->redirect_ = obj.redirect_;
  return *this;
}

bool LocationSettings::setValue(std::string key, std::string value) {
  value.erase(value.size() - 1);
  if (key == "root") {
    this->root_ = value;
  } else if (key == "endpoint") {
    this->endpoint_ = value;
  } else if (key == "default") {
    this->default_ = value;
  } else if (key == "auto-index") {
    this->auto_index_ = value == "true" ? true : false;
  } else if (key == "allow-method") {
    this->allowed_methods_.push_back(value);
  } else if (key == "allow-upload") {
    this->allow_upload_ = value == "true" ? true : false;
  } else if (key == "upload-directory") {
    this->upload_dir_ = value;
  } else if (key == "redirect") {
    this->redirect_ = value;
  } else {
    return false;
  }
  return true;
}

const std::vector<std::string>& LocationSettings::getAllowedMethods() const {
  return this->allowed_methods_;
}

std::string LocationSettings::getRedir() const { return this->redirect_; }

std::string LocationSettings::getRoot() const { return this->root_; }

std::string LocationSettings::getEndpoint() const { return this->endpoint_; }

std::string LocationSettings::getDefault() const { return this->default_; }

bool LocationSettings::getAutoIndex() const { return this->auto_index_; }

bool LocationSettings::getAllowUpload() const { return this->allow_upload_; }

std::string LocationSettings::getUploadDir() const { return this->upload_dir_; }
