#include "TcpServer.hpp"
#include "parser/SettingsParser.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  std::string conf_path("./config/default.conf");
  SettingsParser settings(conf_path);
  // this is just an example for now - as soon as we know which settings to
  // parse i'd suggest to switch from settings maps to proper classes with
  // member values
  TcpServer server = TcpServer(
      "0.0.0.0",
      std::strtod(
          settings.parsed_settings_.settings_.find("port")->second.c_str(),
          NULL));
  server.run();
  return 0;
}
/*
 * TcpServer("0.0.0.0", settings);
 *
 * inside TcpServer:
 *
 * for server in settings.servers
 *  listen to server.port
 */
