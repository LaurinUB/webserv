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
