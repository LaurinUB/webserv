#include "Parser.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

Parser::Parser() {}

Parser::~Parser() {}

Parser::Parser(const Parser& obj) : global(obj.global), tokens_(obj.tokens_) {}

Parser& Parser::operator=(const Parser& obj) {
  this->global = obj.global;
  this->tokens_ = obj.tokens_;
  return *this;
}

Parser::token_type Parser::identifyTokenType(std::string& token) {
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
  int bracket_count = 0;
  while (std::getline(file_stream, line)) {
    std::string token;
    std::istringstream iss(line);
    while (iss >> token) {
      if (*token.begin() == '#') {
        break;
      }
      tokenized_file.push_back(std::pair<std::string, token_type>(
          token, this->identifyTokenType(token)));
      if (this->identifyTokenType(token) == OPEN_CBR_TOKEN) {
        bracket_count += 1;
      } else if (this->identifyTokenType(token) == CLOSE_CBR_TOKEN) {
        bracket_count -= 1;
      }
    }
  }
  this->tokens_ = tokenized_file;
  if (this->tokens_.empty()) {
    throw std::runtime_error("empty config");
  } else if (this->tokens_[0].second != Parser::HTTP_TOKEN) {
    throw std::runtime_error("missing http block");
  } else if (bracket_count != 0) {
    throw std::runtime_error("unclosed block(s)");
  }
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
    } else if (it->second == ROUTE_TOKEN) {
      throw std::runtime_error("location setting in wrong scope");
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
      if (!res.setValue((it - 1)->first, it->first)) {
        std::string error("In Server Unknown key: " + (it - 1)->first);
        throw std::runtime_error(error.c_str());
      }
    } else if (it->second == SERVER_TOKEN) {
      throw std::runtime_error("Nested server not allowed");
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
      if (!res.setValue((it - 1)->first, it->first)) {
        std::string error("In Location Unknown key: " + (it - 1)->first);
        throw std::runtime_error(error.c_str());
      }
    } else if (it->second == SERVER_TOKEN || it->second == ROUTE_TOKEN) {
      throw std::runtime_error("Nested location not allowed");
    }
  }
  return res;
}
