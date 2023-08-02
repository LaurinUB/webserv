#include "parser/SettingsParser.hpp"
#include "TcpServer.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  std::string conf_path("./config/default.conf");
  SettingsParser settings(conf_path);
  TcpServer server = TcpServer(
      "0.0.0.0",
      std::strtod(
          settings.parsed_settings_.settings_.find("port")->second.c_str(),
          NULL));
  server.run();
  return 0;
}
