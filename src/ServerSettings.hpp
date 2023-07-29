#ifndef SERVERSETTINGS_HPP_
#define SERVERSETTINGS_HPP_

#include <string>

class ServerSettings { 
 public:
  ServerSettings();
  ~ServerSettings();
  ServerSettings(const ServerSettings& obj);
  ServerSettings& operator=(const ServerSettings& obj);
  ServerSettings(std::string& config_path);

  typedef enum {
    END_TOKEN,
    DEFAULT_TOKEN,
    OPEN_CBR_TOKEN,
    CLOSE_CBR_TOKEN,
  } token_type;

 private:
  unsigned int port_;
};

#endif  // SERVERSETTINGS_HPP_
