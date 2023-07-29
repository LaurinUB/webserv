#include "ServerSettings.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

ServerSettings::ServerSettings() {}

ServerSettings::~ServerSettings() {}

ServerSettings::ServerSettings(const ServerSettings& obj) : port_(obj.port_) {}

ServerSettings& ServerSettings::operator=(const ServerSettings& obj) {
  this->port_ = obj.port_;
  return *this;
}

ServerSettings::ServerSettings(std::string& config_path) {
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
      tokenized_file.push_back(
          std::pair<std::string, token_type>(token, DEFAULT_TOKEN));
    }
  }
  for (std::vector<std::pair<std::string, token_type> >::iterator i =
           tokenized_file.begin();
       i != tokenized_file.end(); ++i) {
    std::cout << i->first << "---" << i->second << std::endl;
  }
}
