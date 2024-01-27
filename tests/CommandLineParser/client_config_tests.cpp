#include "test_framework_for_parser.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

std::unique_ptr<util::Config> GetClientConfig(std::vector<std::pair<std::string, int>> &addresses,
                                              std::vector<std::string> &packages);
void TestClient(std::vector<std::pair<std::string, int>> &addresses, std::vector<std::string> &packages);

SCENARIO("Client config", "[CommandLineParser]") {
  SECTION("Correct program options") {
    std::vector<std::pair<std::string, int>> addresses;
    std::vector<std::string> packages;

    SECTION("One receiver - One package") {
      addresses = {{"127.0.0.1"s, 3333}};
      packages = {"img.png"s};

      TestClient(addresses, packages);
    }

    SECTION("One receiver - five packages") {
      addresses = {{"127.0.0.1"s, 3333}};
      packages = {"img.png"s, ".gitignore"s, "README.md"s, "conanfile.txt"s, "package.txt"s};

      TestClient(addresses, packages);
    }

    SECTION("Five receivers - one package") {
      addresses = {{"127.0.0.1"s, 3333}, {"192.0.0.1"s, 3333}, {"127.1.0.1"s, 3334}, {"127.0.2.1"s, 3335},
                   {"127.0.0.1"s, 3336}};
      packages = {"img.png"s};

      TestClient(addresses, packages);
    }

    SECTION("Five receivers - five packages") {
      addresses = {{"127.0.0.1"s, 3333}, {"192.0.0.1"s, 3333}, {"127.1.0.1"s, 3334}, {"127.0.2.1"s, 3335},
                   {"127.0.0.1"s, 3336}};
      packages = {"img.png"s, ".gitignore"s, "README.md"s, "conanfile.txt"s, "package.txt"s};

      TestClient(addresses, packages);
    }
  }

  SECTION("Options with mistakes") {
    std::vector<std::pair<std::string, int>> addresses;
    std::vector<std::string> packages;

    SECTION("Negative port number") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "127.0.0.1/-3333", "127.0.1.1/-334",
           "--packages", "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::NON_EXISTENT_PORT.data());
    }

    SECTION("Empty packages") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.1/3333", "127.0.0.2/3233", "--packages"};
      CHECK_THROWS(ParseFromVector(arguments));
    }

    SECTION("Empty addresses") {
      std::vector<std::string> arguments = {"program_name", "--mode", "client", "--addresses", "--packages", "img.png"};
      CHECK_THROWS(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }

    SECTION("Only one empty address") {
      std::vector<std::string>
          arguments = {"program_name", "--mode", "client", "--addresses", "", "--packages", "img.png"};
      CHECK_THROWS(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }

    SECTION("Missed port in address") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "127.0.0.1/", "--packages", "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }

    SECTION("Letters in port number") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "127.0.0.1/bar", "--packages",
           "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }

    SECTION("Only IP in address") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "127.0.0.1", "--packages", "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }

    SECTION("Wrong IP format") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "434053.0.0.1/3321", "--packages",
           "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::NON_EXISTENT_IP.data());
    }

    SECTION("Missed ip in address") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "/3333", "--packages", "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }

    SECTION("Without delimiter") {
      std::vector<std::string> arguments =
          {"program_name", "--mode", "client", "--addresses", "127.0.0.2/3233", "127.0.0.1 3333", "--packages",
           "img.png"};
      CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
    }
  }
}

std::unique_ptr<util::Config> GetClientConfig(std::vector<std::pair<std::string, int>> &addresses,
                                              std::vector<std::string> &packages) {
  std::vector<std::string> arguments = {"program_name", "--mode", "client", "--addresses"};
  arguments.reserve(addresses.size() + packages.size() + 1);

  for (auto &address : addresses) {
    arguments.push_back(address.first + "/"s + std::to_string(address.second));
  }

  arguments.emplace_back("--packages");
  arguments.insert(arguments.end(), packages.begin(), packages.end());

  return ParseFromVector(arguments);
}

void TestClient(std::vector<std::pair<std::string, int>> &addresses, std::vector<std::string> &packages) {
  auto config = GetClientConfig(addresses, packages);
  auto &client_config = dynamic_cast<util::ClientConfig &>(*config);

  REQUIRE(client_config.mode == util::Mode::Client);
  REQUIRE(client_config.path_to_packages.size() == packages.size());
  REQUIRE(client_config.servers.size() == addresses.size());

  for (size_t i = 0; i < addresses.size(); ++i) {
    CHECK(addresses[i].first == client_config.servers[i].first);
    CHECK(addresses[i].second == client_config.servers[i].second);
  }

  for (size_t i = 0; i < packages.size(); ++i) {
    CHECK(client_config.path_to_packages[i] == packages[i]);
  }
}

/*
 * Тест кейсы!
 * 1) Проверка работы исключений
 *     - Неправильная конфигурация сервера
 *     - Неправильная конфигурация клиента Сделано!!!!!!!!!!!!!!!!
 *     - Пропущено поле Mode
 *     - Неизвестный режим работы программы
 *
 *
 * 2) Проверка верности парсинга
 *    - сервера
 *    - клиента Сделано!!!!!!!!!!!!!!!!!!!!!!!!!!
 *    - None
 *
 */