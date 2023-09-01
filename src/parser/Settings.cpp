#include "Settings.hpp"

#include "Parser.hpp"

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
  try {
    Parser parsed(config_path);
    *this = parsed.global;
  } catch (std::runtime_error e) {
    std::cout << "Parser Exception: " << e.what() << std::endl;
    throw std::exception();
  }
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
      this->getServers()[server_idx].getRoutes()[route_idx].getAllowedMethods();
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
  return this->getServers()[server_idx].getRoutes()[route_idx].getAutoIndex();
}

bool Settings::getRouteAllowUpload(unsigned int server_idx,
                                   unsigned int route_idx) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->getServers()[server_idx].getRoutes()[route_idx].getAllowUpload();
}

std::string Settings::getRouteUploadDir(unsigned int server_idx,
                                        unsigned int route_idx) const {
  if (server_idx >= this->getServers().size() ||
      route_idx >= this->getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->getServers()[server_idx].getRoutes()[route_idx].getUploadDir();
}

unsigned int Settings::matchServer(int port) const {
  unsigned int res = 0;
  unsigned int i = 0;
  for (std::vector<ServerSettings>::const_iterator it = this->servers.begin();
       it != this->servers.end(); ++it) {
    if ((int) it->getPort() == port) {
      res = i;
      break;
    }
    i++;
  }
  return res;
}

unsigned int Settings::matchLocationOfServer(
    unsigned int server_idx, const std::string& endpoint) const {
  unsigned int res = 0;
  for (std::vector<LocationSettings>::const_iterator it =
           this->getServers()[server_idx].locations.begin();
       it != this->getServers()[server_idx].locations.end(); ++it) {
    size_t found_at = endpoint.find(it->getEndpoint());
    if (found_at == 0) {
      break;
    }
    res++;
  }
  return res;
}
