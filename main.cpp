#include "TcpServer.hpp"

int main() {
  TcpServer server = TcpServer("0.0.0.0", 6969);
  server.startListen();
  return 0;
}
