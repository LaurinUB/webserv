#ifndef SETTINGS_PARSER_
#define SETTINGS_PARSER_

#include <string>
#include <vector>

#include "../HTTPRequest.hpp"
#include "GlobalSettings.hpp"
#include "LocationSettings.hpp"
#include "ServerSettings.hpp"

class SettingsParser {
 public:
  SettingsParser();
  ~SettingsParser();
  SettingsParser(const SettingsParser& obj);
  SettingsParser& operator=(const SettingsParser& obj);
  SettingsParser(std::string& config_path);

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
  GlobalSettings parseHTTP();
  VServerSettings parseServer(
      std::vector<std::pair<std::string, token_type> >::iterator& it);
  LocationSettings parseRoute(
      std::vector<std::pair<std::string, token_type> >::iterator& it);
  GlobalSettings parsed_settings_;
};

#endif  // SETTINGS_PARSER_
