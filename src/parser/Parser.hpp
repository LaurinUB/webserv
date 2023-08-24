#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include <vector>

#include "../HTTPRequest.hpp"
#include "LocationSettings.hpp"
#include "ServerSettings.hpp"
#include "Settings.hpp"

class Parser {
 public:
  Parser();
  ~Parser();
  Parser(const Parser& obj);
  Parser& operator=(const Parser& obj);
  Parser(std::string& config_path);

  Settings global;

  bool isMethodAllowedOnRoute(unsigned int server_idx, unsigned int route_idx,
                              std::string method) const;
  std::string getRouteRoot(unsigned int server_idx,
                           unsigned int route_idx) const;
  bool getRouteAutoIndex(unsigned int server_idx, unsigned int route_idx) const;
  bool getRouteAllowUpload(unsigned int server_idx,
                           unsigned int route_idx) const;
  std::string getRouteUploadDir(unsigned int server_idx,
                                unsigned int route_idx) const;

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
  Settings parseHTTP();
  ServerSettings parseServer(
      std::vector<std::pair<std::string, token_type> >::iterator& it);
  LocationSettings parseRoute(
      std::vector<std::pair<std::string, token_type> >::iterator& it);
};

#endif  // PARSER_HPP_
