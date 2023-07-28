#include "ServerSettings.hpp"

#include <fstream>
#include <iostream>

ServerSettings::ServerSettings() {}

ServerSettings::~ServerSettings() {}

ServerSettings::ServerSettings(const ServerSettings& obj) : port_(obj.port_) {}

ServerSettings& ServerSettings::operator=(const ServerSettings& obj) {
  this->port_ = obj.port_;
  return *this;
}

ServerSettings::ServerSettings(std::string& config_path) {
  std::ifstream file_stream(config_path);
  std::string line;
  while (std::getline(file_stream, line)) {
    std::cout << line << std::endl;
  }
}
