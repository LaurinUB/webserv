#include "Parser.hpp"
#include "Settings.hpp"

Settings::Settings(){};

Settings::~Settings(){};

Settings::Settings(const Settings& obj) : servers(obj.servers) {
  this->settings_ = obj.settings_;
};

Settings& Settings::operator=(Settings obj) {
  this->settings_ = obj.settings_;
  this->servers = obj.servers;
  return *this;
}

Settings::Settings(std::string& config_path) {
  Parser parsed(config_path);
  *this = parsed.global;
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

bool Settings::isMethodAllowedOnRoute(unsigned int server_idx,
                                            unsigned int route_idx,
                                            std::string method) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  std::vector<std::string> allowed_methods =
      this->getServers()[server_idx]
          .getRoutes()[route_idx]
          .getAllowedMethods();
  if (std::find(allowed_methods.begin(), allowed_methods.end(), method) !=
      allowed_methods.end()) {
    return true;
  }
  return false;
}

std::string Settings::getRouteRoot(unsigned int server_idx,
                                         unsigned int route_idx) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->getServers()[server_idx].getRoutes()[route_idx].getRoot();
}

bool Settings::getRouteAutoIndex(unsigned int server_idx,
                                       unsigned int route_idx) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->getServers()[server_idx]
      .getRoutes()[route_idx]
      .getAutoIndex();
}

bool Settings::getRouteAllowUpload(unsigned int server_idx,
                                         unsigned int route_idx) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->getServers()[server_idx]
      .getRoutes()[route_idx]
      .getAllowUpload();
}

std::string Settings::getRouteUploadDir(unsigned int server_idx,
                                              unsigned int route_idx) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->getServers()[server_idx]
      .getRoutes()[route_idx]
      .getUploadDir();
}
