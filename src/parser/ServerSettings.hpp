#ifndef SERVERSETTINGS_HPP_
#define SERVERSETTINGS_HPP_

#include <map>
#include <string>
#include <vector>

#include "ASettings.hpp"
#include "LocationSettings.hpp"

class ServerSettings : public ASettings {
  friend class Parser;

 public:
  ServerSettings();
  virtual ~ServerSettings();
  ServerSettings(const ServerSettings& obj);
  ServerSettings& operator=(const ServerSettings& obj);

  bool setValue(std::string key, std::string value);

  std::vector<LocationSettings> getRoutes() const;
  unsigned int getPort() const;
  std::string getName() const;
  std::string getListen() const;
  std::map<unsigned int, std::string> getErrorPages() const;
  unsigned int getMaxClientBodySize() const;

  unsigned int matchLocation(const std::string& endpoint) const;

 private:
  std::vector<LocationSettings> locations;
  unsigned int port_;
  std::string listen_;
  std::string server_name_;
  std::map<unsigned int, std::string> error_pages_;
  unsigned int max_client_body_size_;
};

#endif  // SERVERSETTINGS_HPP_
