#include "ServerSettings.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

ServerSettings::ServerSettings()
    : server_name_("0.0.0.0"),
      port_(6969),
      default_errorpage_("./data/404.html"),
      accepted_methods_(HTTPRequest::GET, HTTPRequest::HEAD),
      auto_index_(true)
{}

ServerSettings::~ServerSettings() {}

ServerSettings::ServerSettings(const ServerSettings& obj) : port_(obj.port_) {}

ServerSettings& ServerSettings::operator=(const ServerSettings& obj) {
  this->port_ = obj.port_;
  return *this;
}

ServerSettings::token_type ServerSettings::identifyToken(std::string& token) {
  if (token == "{") {
    return ServerSettings::OPEN_CBR_TOKEN;
  } else if (token == "}") {
    return ServerSettings::CLOSE_CBR_TOKEN;
  } else if (token == "=") {
    return ServerSettings::EXACT_LOCATION_TOKEN;
  } else if (*(token.end() - 1) == ';') {
    return ServerSettings::VALUE_TOKEN;
  } else {
    return ServerSettings::SETTING_TOKEN;
  }
}

ServerSettings::ServerSettings(std::string& config_path) {
  std::string types[6] = {
    "UNKNOWN_TOKEN",
    "SETTING_TOKEN",
    "VALUE_TOKEN",
    "OPEN_CBR_TOKEN",
    "CLOSE_CBR_TOKEN",
    "EXACT_LOCATION_TOKEN"
  };
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
          token, this->identifyToken(token)));
    }
  }
 // std::vector<TreeNode*> ast_root;
 // std::pair<std::string, ServerSettings::token_type> prev_token;
 // for (std::vector<std::pair<std::string, token_type> >::iterator i =
 //          tokenized_file.begin();
 //      i != tokenized_file.end(); ++i) {
 //   std::cout << i->first << " " << types[i->second] << std::endl;
 //   prev_token = *i;
 // }
}
