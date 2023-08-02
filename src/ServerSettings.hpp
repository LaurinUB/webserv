#ifndef SERVERSETTINGS_HPP_
#define SERVERSETTINGS_HPP_

#include <string>
#include <vector>

#include "GlobalSettings.hpp"
#include "HTTPRequest.hpp"
#include "LocationSettings.hpp"
#include "VServerSettings.hpp"

class ServerSettings { 
 public:
  ServerSettings();
  ~ServerSettings();
  ServerSettings(const ServerSettings& obj);
  ServerSettings& operator=(const ServerSettings& obj);
  ServerSettings(std::string& config_path);

  typedef enum {
    UNKNOWN_TOKEN,
    SETTING_TOKEN,
    VALUE_TOKEN,
    OPEN_CBR_TOKEN,
    CLOSE_CBR_TOKEN,
    SERVER_TOKEN,
    ROUTE_TOKEN,
    HTTP_TOKEN
  } token_type;

 private:
  //// private methods
  token_type identifyTokenType(std::string& token);
  std::vector<std::pair<std::string, token_type> > tokens_;
  void parse();
  void parseHTTP();
  VServerSettings parseServer(
      std::vector<std::pair<std::string, token_type> >::iterator& it);
  LocationSettings parseRoute(
      std::vector<std::pair<std::string, token_type> >::iterator& it);
};

#endif  // SERVERSETTINGS_HPP_
