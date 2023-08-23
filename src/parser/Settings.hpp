#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <vector>

#include "ASettings.hpp"
#include "ServerSettings.hpp"

class Settings : public ASettings {
 public:
  Settings();
  virtual ~Settings();
  Settings(const Settings& obj);
  Settings& operator=(Settings obj);

  void addServer(ServerSettings server);
  bool setValue(std::string key, std::string value);
  const std::vector<ServerSettings> getServers() const;

 private:
  std::vector<ServerSettings> servers;
};

#endif  // SETTINGS_HPP_
