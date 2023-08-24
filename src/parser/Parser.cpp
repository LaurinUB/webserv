#include "Parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Parser::Parser() {}

Parser::~Parser() {}

Parser::Parser(const Parser& obj)
    : global(obj.global), tokens_(obj.tokens_) {}

Parser& Parser::operator=(const Parser& obj) {
  this->global = obj.global;
  this->tokens_ = obj.tokens_;
  return *this;
}

Parser::token_type Parser::identifyTokenType(
    std::string& token) {
  if (token == "{") {
    return Parser::OPEN_CBR_TOKEN;
  } else if (token == "}") {
    return Parser::CLOSE_CBR_TOKEN;
  } else if (token == "server") {
    return Parser::SERVER_TOKEN;
  } else if (token == "http") {
    return Parser::HTTP_TOKEN;
  } else if (token == "location") {
    return Parser::ROUTE_TOKEN;
  } else if (*(token.end() - 1) == ';') {
    return Parser::VALUE_TOKEN;
  } else {
    return Parser::SETTING_TOKEN;
  }
}

Parser::Parser(std::string& config_path) {
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
  this->global = parseGlobal();
}

Settings Parser::parseGlobal() {
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

ServerSettings Parser::parseServer(
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

LocationSettings Parser::parseRoute(
    std::vector<std::pair<std::string, token_type> >::iterator& it) {
  LocationSettings res;
  for (; it->second != CLOSE_CBR_TOKEN; ++it) {
    if (it->second == VALUE_TOKEN) {
      res.setValue((it - 1)->first, it->first);
    }
  }
  return res;
}
