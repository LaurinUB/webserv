#include "ServerSettings.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

ServerSettings::ServerSettings() {}

ServerSettings::~ServerSettings() {}

ServerSettings::ServerSettings(const ServerSettings& obj)
    : tokens_(obj.tokens_) {}

ServerSettings& ServerSettings::operator=(const ServerSettings& obj) {
  this->tokens_ = obj.tokens_;
  return *this;
}

ServerSettings::token_type ServerSettings::identifyTokenType(std::string& token) {
  if (token == "{") {
    return ServerSettings::OPEN_CBR_TOKEN;
  } else if (token == "}") {
    return ServerSettings::CLOSE_CBR_TOKEN;
  } else if (token == "server") {
    return ServerSettings::SERVER_TOKEN;
  } else if (token == "http") {
    return ServerSettings::HTTP_TOKEN;
  } else if (token == "location") {
    return ServerSettings::ROUTE_TOKEN;
  } else if (*(token.end() - 1) == ';') {
    return ServerSettings::VALUE_TOKEN;
  } else {
    return ServerSettings::SETTING_TOKEN;
  }
}

ServerSettings::ServerSettings(std::string& config_path) {
  std::string types[8] = {
    "UNKNOWN_TOKEN",
    "SETTING_TOKEN",
    "VALUE_TOKEN",
    "OPEN_CBR_TOKEN",
    "CLOSE_CBR_TOKEN",
    "SERVER_TOKEN",
    "ROUTE_TOKEN",
    "HTTP_TOKEN"
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
          token, this->identifyTokenType(token)));
    }
  }
  for (std::vector<std::pair<std::string, token_type> >::iterator i =
           tokenized_file.begin();
       i != tokenized_file.end(); ++i) {
    //std::cout << i->first << " " << types[i->second] << std::endl;
  }
  this->tokens_ = tokenized_file;
  this->parse();
}

// each parsing function should take the remaining tokens and add key
// values while removing corresponding tokens from the token vector
void ServerSettings::parseHTTP() {
  GlobalSettings res;
  // iterate over tokens, if there is a server token:
  // res.settings_.push_back(parseServer())
  token_type previous = UNKNOWN_TOKEN;
  for (std::vector<std::pair<std::string, token_type> >::iterator it =
           this->tokens_.begin();
       it != this->tokens_.end(); ++it) {
    if (previous == SERVER_TOKEN && it->second == OPEN_CBR_TOKEN) {
      //std::cout << "parsing VServerSettings" << std::endl;
      res.server_settings_.push_back(this->parseServer(it));
    } else if (it->second == VALUE_TOKEN) {
      res.settings_.insert(
          std::pair<std::string, std::string>((it - 1)->first, it->first));
      std::cout << "GLOBAL ADDED: " << (it - 1)->first << ": " << it->first
                << std::endl;
    }
    previous = it->second;
  }
  // return res;
}

VServerSettings ServerSettings::parseServer(
    std::vector<std::pair<std::string, token_type> >::iterator& it) {
  VServerSettings res;
  // iterate over tokens, if there is a location token:
  // res.settings_.psh_back(parseRoute())
  for (;it->second != CLOSE_CBR_TOKEN; ++it) {
    if (it->second == VALUE_TOKEN) {
      res.settings_.insert(
          std::pair<std::string, std::string>((it - 1)->first, it->first));
      std::cout << "SERVER ADDED: " << (it - 1)->first << ": " << it->first
                << std::endl;
    } else if (it->second == ROUTE_TOKEN) {
      //std::cout << "parsing RouteSettings" << std::endl;
      parseRoute(it);
    }
  }
  return res;
}
//
LocationSettings ServerSettings::parseRoute(
    std::vector<std::pair<std::string, token_type> >::iterator& it) {
  LocationSettings res;
  // iterate over tokens, add them to this->settings_;
  for (;it->second != CLOSE_CBR_TOKEN; ++it) {
    if (it->second == VALUE_TOKEN) {
      res.settings_.insert(
          std::pair<std::string, std::string>((it - 1)->first, it->first));
      std::cout << "LOCATION ADDED: " << (it - 1)->first << ": " << it->first
                << std::endl;
    }
  }
  return res;
}

void ServerSettings::parse() {
  this->parseHTTP();
 // ServerSettings::token_type current = UNKNOWN_TOKEN;
 // for (std::vector<
 //          std::pair<std::string, ServerSettings::token_type> >::iterator it =
 //          this->tokens_.begin();
 //      it != this->tokens_.end(); ++it) {
 //   current = it->second;
 //   std::cout << it->first << std::endl;
 //   if (current == ServerSettings::HTTP_TOKEN) {
 //     it++;
 //     current = it->second;
 //     std::cout << it->first << std::endl;
 //     while (current != ServerSettings::CLOSE_CBR_TOKEN) {
 //       std::cout << "IN HTTP LOOP" << std::endl;
 //       it++;
 //       current = it->second;
 //       std::cout << it->first << std::endl;
 //       if (current == ServerSettings::SERVER_TOKEN) {
 //         it++;
 //         current = it->second;
 //         std::cout << it->first << std::endl;
 //         while (current != ServerSettings::CLOSE_CBR_TOKEN) {
 //           std::cout << "IN SRVER LOOP" << std::endl;
 //           it++;
 //           current = it->second;
 //           std::cout << it->first << std::endl;
 //           if (current == ServerSettings::ROUTE_TOKEN) {
 //             it++;
 //             current = it->second;
 //             std::cout << it->first << std::endl;
 //             while (current != ServerSettings::CLOSE_CBR_TOKEN) {
 //               std::cout << "IN ROUTe LOOP" << std::endl;
 //               it++;
 //               current = it->second;
 //               std::cout << it->first << std::endl;
 //             }
 //           }
 //         }
 //       }
 //     }
 //   }
 // }
}
