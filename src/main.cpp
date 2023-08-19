#include "Server.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main() {
  signal(SIGINT, handleSIGINT);
  Server server = Server("0.0.0.0", PORT);
  server.run();
  return 0;
}
