#include "Server.hpp"
#include "parser/Settings.hpp"

sig_atomic_t g_signaled = 0;

void handleSIGINT(int param) {
  g_signaled = param;
  _exit(SIGINT);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, handleSIGINT);
  std::string conf_path;
  if (argc == 1) {
    conf_path = "./config/test.conf";
  } else if (argc == 2) {
    conf_path = argv[1];
  } else {
    std::cerr << "Error: wrong amount of arguments" << std::endl;
    return EXIT_FAILURE;
  }
  try {
    Settings settings;
    if (argc == 2) {
      settings = Settings(conf_path);
    }
    Server server = Server(settings);
    server.run();
  } catch (std::exception e) {
    e.what();
  }
  return 0;
}
