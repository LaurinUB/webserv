#include "TcpServer.hpp"

sig_atomic_t signaled = 0;

void handleSIGINT(int param) {
  (void) param;
  signaled = 1;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  TcpServer server = TcpServer("0.0.0.0", 6969);
  server.startListen();
  return 0;
}
