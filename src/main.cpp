#include <iostream>


#include "util/logger/logger.h"
#include "util/Parser/command_line_parser.h"
#include "app/Application.h"

using namespace std::literals;
using config::Mode;

int main(int argc, char *argv[]) {
  // TODO Душа требует нормального логирования
  // TODO Обработку сигналов бы еще

  LoggerInit();

  try {
    app::Application app;
    util::CommandLineParser parser;

    auto config = parser.ParseCommandLine(argc, argv);

    if (config->mode == Mode::SERVER) {
      auto &server_config = dynamic_cast<config::ServerConfig &>(*config);

      app.Listen(server_config.port, server_config.public_key_path, server_config.private_key_path);
    } else if (config->mode == Mode::CLIENT) {
      auto &client_config = dynamic_cast<config::ClientConfig &>(*config);

      app.Send(client_config.servers.at(0).first,
               client_config.servers.at(0).second,
               client_config.path_to_packages.at(0));
    }

    return EXIT_SUCCESS;
  }
  catch (const std::exception &ex) {
    LogFatal(ex.what());

    return EXIT_FAILURE;
  }
}
