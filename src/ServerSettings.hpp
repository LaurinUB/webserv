#ifndef SERVERSETTINGS_HPP_
#define SERVERSETTINGS_HPP_

#include <string>
#include <vector>

#include "HTTPRequest.hpp"

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
    EXACT_LOCATION_TOKEN
  } token_type;

  std::string server_name_;
  unsigned int port_;
  std::string default_errorpage_;
  std::vector<HTTPRequest::method> accepted_methods_;
  bool auto_index_;

 private:
  //// private methods
  token_type identifyToken(std::string& token);
};

//class TreeNode {
// public:
//  TreeNode(std::pair<std::string, ServerSettings::token_type> content);
//  std::pair<std::string, ServerSettings::token_type> content_;
//  TreeNode* parent_;
//  std::vector<TreeNode*> children_;
//  void addChild(std::pair<std::string, ServerSettings::token_type> new_content);
//};
//
//TreeNode::TreeNode(std::pair<std::string, ServerSettings::token_type> content)
//    : content_(content) {}

#endif  // SERVERSETTINGS_HPP_
