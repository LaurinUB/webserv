#include "TcpServer.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  TcpServer server = TcpServer("127.0.0.1", PORT);
  server.startListen();
  return 0;
}
