#include "parser/SettingsParser.hpp"
#include "TcpServer.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  std::string conf_path("./config/test.conf");
  SettingsParser settings(conf_path);
  TcpServer server = TcpServer("0.0.0.0", PORT);
  server.run();
  return 0;
}
