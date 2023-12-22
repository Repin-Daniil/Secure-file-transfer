#include <iostream>
#include "Application/Application.h"

using namespace std::literals;

int main(int argc, char **argv) {
  app::Application app;

  std::string pub_key = "-----BEGIN PUBLIC KEY-----\n"
                        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC98Uf7CSPaBm0v/Cc1sEImSjl0\n"
                        "NgXKPjbMaKKCFtrdUzB4Kl6PN67MVwUVcOV22oljxEnM+wlqfMb45vfFDP/S3EtL\n"
                        "4zRDLzlJIRV33IzZpIExUX4G0ZCe8tyVZ2Kdc7+pyQdlDxSwM8VylylwgUCbfIcV\n"
                        "WvltjaAsln8pA5mZBwIDAQAB\n"
                        "-----END PUBLIC KEY-----";

  try {
    if (argc == 5 && argv[1] == "server"sv) {
      app.Listen(*argv[2], argv[3], argv[4]);
    } else if (argc == 5 && argv[1] == "client"sv) {
      app.Send(argv[2], static_cast<unsigned int>(*argv[3]), argv[4]);
    } else {
      std::cout << "Usage: "sv << argv[0] << " server <port>, <RSA-pubkey-path>, <RSA-private-key-path>"sv << std::endl;
      std::cout << "Usage: "sv << argv[0] << " client <server IP>, <server port>, <file path>" << std::endl;

      return 1;
    }
  }
  catch (const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  return 0;
}
