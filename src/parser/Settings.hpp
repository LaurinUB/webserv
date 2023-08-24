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
  Settings(std::string& config_path);

  void addServer(ServerSettings server);
  bool setValue(std::string key, std::string value);
  const std::vector<ServerSettings> getServers() const;
  bool isMethodAllowedOnRoute(unsigned int server_idx, unsigned int route_idx,
                              std::string method) const;
  std::string getRouteRoot(unsigned int server_idx,
                           unsigned int route_idx) const;
  bool getRouteAutoIndex(unsigned int server_idx, unsigned int route_idx) const;
  bool getRouteAllowUpload(unsigned int server_idx,
                           unsigned int route_idx) const;
  std::string getRouteUploadDir(unsigned int server_idx,
                                unsigned int route_idx) const;

 private:
  std::vector<ServerSettings> servers;
};

#endif  // SETTINGS_HPP_