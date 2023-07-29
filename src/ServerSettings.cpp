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
  std::vector<std::string> tokenized_file;
  while (std::getline(file_stream, line)) {
    std::string token;
    std::istringstream iss(line);
    while (iss >> token) {
      if (*token.begin() == '#') {
        break;
      }
      tokenized_file.push_back(token);
    }
  }
  std::cout << "Tokenized file:" << std::endl;
  for (std::vector<std::string>::iterator i = tokenized_file.begin();
       i != tokenized_file.end(); ++i) {
    std::cout << *i << std::endl;
  }
}
