#include "Settings.hpp"

#include "Parser.hpp"

Settings::Settings() {}

Settings::~Settings() {}

Settings::Settings(const Settings& obj) : servers(obj.servers) {
  this->settings_ = obj.settings_;
}

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
  // in case there is no server, create a default one
  if (this->servers.size() == 0) {
    ServerSettings default_server;
    this->servers.push_back(default_server);
  }
  // in case a server has no route create a default one, no allowed method
  // specified ? add GET as default
  for (std::vector<ServerSettings>::iterator it = this->servers.begin();
       it != this->servers.end(); ++it) {
    if (it->getRoutes().size() == 0) {
      LocationSettings default_location;
      it->locations.push_back(default_location);
    }
    for (std::vector<LocationSettings>::iterator lt = it->locations.begin();
         lt != it->locations.end(); ++lt) {
      if (lt->allowed_methods_.empty()) {
        lt->allowed_methods_.push_back("GET");
      }
    }
  }
}

void Settings::addServer(ServerSettings server) {
  this->servers.push_back(server);
}

bool Settings::setValue(std::string key, std::string value) {
  (void)key;
  (void)value;
  return true;
}

const std::vector<ServerSettings> Settings::getServers() const {
  return this->servers;
}

unsigned int Settings::matchServer(int port, const std::string& name) const {
  unsigned int res = 0;
  unsigned int i = 0;
  for (std::vector<ServerSettings>::const_iterator it = this->servers.begin();
       it != this->servers.end(); ++it) {
    if ((int)it->getPort() == port && it->getName() == name) {
      res = i;
      break;
    } else if ((int)it->getPort() == port) {
      res = i;
    }
    i++;
  }
  return res;
}
