#include <iostream>
#include <memory>
#include <condition_variable>

#include "Application/Application.h"
#include "util/command_line_parser.h"

using namespace std::literals;

int main(int argc, char* argv[]) {

  //TODO Добавить JSON конфиг
  //TODO Душа требует нормального логирования
  //TODO Сервер слишком наивный, больше асинхронщины!
  //TODO Broadcast наше все + отправка многих посылок за раз (парсер уже в курсе)
  //TODO Обработку сигналов бы еще


  try {
    app::Application app;
    util::CommandLineParser parser;

    auto config = parser.ParseCommandLine(argc, argv);

    if (config->mode == util::Server) {
      auto &server_config = dynamic_cast<util::ServerConfig &>(*config);

      app.Listen(server_config.port, server_config.public_key_path, server_config.private_key_path);
    } else if (config->mode == util::Client) {
      auto &client_config = dynamic_cast<util::ClientConfig &>(*config);

      app.Send(client_config.servers.at(0).first,
               client_config.servers.at(0).second,
               client_config.path_to_packages.at(0));
    }

    return EXIT_SUCCESS;
  }
  catch (const std::exception &ex) {
    std::cout << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
}
