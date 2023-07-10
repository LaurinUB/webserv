#include "TcpServer.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  (void)param;
  g_signaled = 1;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  TcpServer server = TcpServer("0.0.0.0", 6969);
  server.startListen();
  return 0;
}
