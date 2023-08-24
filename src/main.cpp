#include "TcpServer.hpp"
#include "parser/Settings.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  std::string conf_path("./config/default.conf");
  Settings settings(conf_path);
  TcpServer server = TcpServer(settings);
  server.run();
  return 0;
}
