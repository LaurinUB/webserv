#include "SettingsParser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

SettingsParser::SettingsParser() {}

SettingsParser::~SettingsParser() {}

SettingsParser::SettingsParser(const SettingsParser& obj)
    : global(obj.global), tokens_(obj.tokens_) {}

SettingsParser& SettingsParser::operator=(const SettingsParser& obj) {
  this->global = obj.global;
  this->tokens_ = obj.tokens_;
  return *this;
}

SettingsParser::token_type SettingsParser::identifyTokenType(
    std::string& token) {
  if (token == "{") {
    return SettingsParser::OPEN_CBR_TOKEN;
  } else if (token == "}") {
    return SettingsParser::CLOSE_CBR_TOKEN;
  } else if (token == "server") {
    return SettingsParser::SERVER_TOKEN;
  } else if (token == "http") {
    return SettingsParser::HTTP_TOKEN;
  } else if (token == "location") {
    return SettingsParser::ROUTE_TOKEN;
  } else if (*(token.end() - 1) == ';') {
    return SettingsParser::VALUE_TOKEN;
  } else {
    return SettingsParser::SETTING_TOKEN;
  }
}

SettingsParser::SettingsParser(std::string& config_path) {
  std::string types[8] = {"UNKNOWN_TOKEN",  "SETTING_TOKEN",   "VALUE_TOKEN",
                          "OPEN_CBR_TOKEN", "CLOSE_CBR_TOKEN", "SERVER_TOKEN",
                          "ROUTE_TOKEN",    "HTTP_TOKEN"};
  std::ifstream file_stream(config_path);
  std::string line;
  std::vector<std::pair<std::string, token_type> > tokenized_file;
  while (std::getline(file_stream, line)) {
    std::string token;
    std::istringstream iss(line);
    while (iss >> token) {
      if (*token.begin() == '#') {
        break;
      }
      tokenized_file.push_back(std::pair<std::string, token_type>(
          token, this->identifyTokenType(token)));
    }
  }
  for (std::vector<std::pair<std::string, token_type> >::iterator i =
           tokenized_file.begin();
       i != tokenized_file.end(); ++i) {
  }
  this->tokens_ = tokenized_file;
  this->global = parseHTTP();
}

Settings SettingsParser::parseHTTP() {
  Settings res;
  token_type previous = UNKNOWN_TOKEN;
  for (std::vector<std::pair<std::string, token_type> >::iterator it =
           this->tokens_.begin();
       it != this->tokens_.end(); ++it) {
    if (previous == SERVER_TOKEN && it->second == OPEN_CBR_TOKEN) {
      res.addServer(this->parseServer(it));
    } else if (it->second == VALUE_TOKEN) {
      res.settings_.insert(
          std::pair<std::string, std::string>((it - 1)->first, it->first));
      std::cout << "GLOBAL ADDED: " << (it - 1)->first << ": " << it->first
                << std::endl;
    }
    previous = it->second;
  }
  return res;
}

ServerSettings SettingsParser::parseServer(
    std::vector<std::pair<std::string, token_type> >::iterator& it) {
  ServerSettings res;
  token_type previous = UNKNOWN_TOKEN;
  for (; it->second != CLOSE_CBR_TOKEN; ++it) {
    if (previous == ROUTE_TOKEN && it->second == OPEN_CBR_TOKEN) {
      res.locations.push_back(parseRoute(it));
    } else if (it->second == VALUE_TOKEN) {
      res.setValue((it - 1)->first, it->first);
    }
    previous = it->second;
  }
  return res;
}

LocationSettings SettingsParser::parseRoute(
    std::vector<std::pair<std::string, token_type> >::iterator& it) {
  LocationSettings res;
  for (; it->second != CLOSE_CBR_TOKEN; ++it) {
    if (it->second == VALUE_TOKEN) {
      res.setValue((it - 1)->first, it->first);
    }
  }
  return res;
}

bool SettingsParser::isMethodAllowedOnRoute(unsigned int server_idx,
                                            unsigned int route_idx,
                                            std::string method) const {
  if (server_idx >= this->global.getServers().size() ||
      route_idx >= this->global.getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  std::vector<std::string> allowed_methods =
      this->global.getServers()[server_idx]
          .getRoutes()[route_idx]
          .getAllowedMethods();
  if (std::find(allowed_methods.begin(), allowed_methods.end(), method) !=
      allowed_methods.end()) {
    return true;
  }
  return false;
}

std::string SettingsParser::getRouteRoot(unsigned int server_idx,
                                         unsigned int route_idx) const {
  if (server_idx >= this->global.getServers().size() ||
      route_idx >= this->global.getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->global.getServers()[server_idx].getRoutes()[route_idx].getRoot();
}

bool SettingsParser::getRouteAutoIndex(unsigned int server_idx,
                                       unsigned int route_idx) const {
  if (server_idx >= this->global.getServers().size() ||
      route_idx >= this->global.getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->global.getServers()[server_idx]
      .getRoutes()[route_idx]
      .getAutoIndex();
}

bool SettingsParser::getRouteAllowUpload(unsigned int server_idx,
                                         unsigned int route_idx) const {
  if (server_idx >= this->global.getServers().size() ||
      route_idx >= this->global.getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->global.getServers()[server_idx]
      .getRoutes()[route_idx]
      .getAllowUpload();
}

std::string SettingsParser::getRouteUploadDir(unsigned int server_idx,
                                              unsigned int route_idx) const {
  if (server_idx >= this->global.getServers().size() ||
      route_idx >= this->global.getServers()[server_idx].getRoutes().size()) {
    throw std::runtime_error("invalid server or route on getRouteRoot call");
  }
  return this->global.getServers()[server_idx]
      .getRoutes()[route_idx]
      .getUploadDir();
}
