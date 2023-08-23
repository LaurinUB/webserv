#include "Settings.hpp"

Settings::Settings() {};

Settings::~Settings() {};

Settings::Settings(const Settings& obj) : servers(obj.servers) {
  this->settings_ = obj.settings_;
};

Settings& Settings::operator=(Settings obj) {
  this->settings_ = obj.settings_;
  this->servers = obj.servers;
  return *this;
}

void Settings::addServer(ServerSettings server) {
  this->servers.push_back(server);
};

bool Settings::setValue(std::string key, std::string value) {
  (void)key;
  (void)value;
  return true;
}

const std::vector<ServerSettings> Settings::getServers() const {
  return this->servers;
};
