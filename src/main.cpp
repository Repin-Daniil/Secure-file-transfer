#include <iostream>
#include "Application/Application.h"

using namespace std::literals;

int main(int argc, char **argv) {
  app::Application app;

  try {
    if (argc == 5 && argv[1] == "server"sv) {
      app.Listen(*argv[2], argv[3], argv[4]);
    } else if (argc == 5 && argv[1] == "client"sv) {
      app.Send(argv[2], static_cast<unsigned int>(*argv[3]), argv[4]);
    } else {
      std::cout << "Usage: "sv << argv[0] << " server <port>, <RSA-pubkey-path>, <RSA-private-key-path>"sv << std::endl;
      std::cout << "Usage: "sv << argv[0] << " client <server IP>, <server port>, <file path>"sv << std::endl;

      return 1;
    }
  }
  catch (const std::exception &ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
