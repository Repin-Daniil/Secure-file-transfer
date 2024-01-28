#include "test_framework_for_parser.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("None-mode", "[CommandLineParser]") {
  std::vector<std::string> arguments = {"program_name", "--help"};
  auto config = ParseFromVector(arguments);
  CHECK(config->mode == util::None);
}

TEST_CASE("Unknown mode", "[CommandLineParser]") {
  std::vector<std::string> arguments = {"program_name", "--mode", "something"};
  CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::UNKNOWN_MODE.data());
}

SCENARIO("Partial config of client") {
  SECTION("Without packages") {
    std::vector<std::string> arguments =
        {"program_name", "--mode", "client",
         "--addresses", "127.0.0.1/3333"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_CLIENT_CONFIG.data());
  }

  SECTION("Without addresses") {
    std::vector<std::string>
        arguments = {"program_name", "--mode", "client",
                     "--packages", "img.png"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_CLIENT_CONFIG.data());
  }

  SECTION("Without mode") {
    std::vector<std::string> arguments = {"program_name",
                                          "--packages", "img.png",
                                          "--addresses", "127.0.0.1/3333"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::OPTION_MODE_IS_MISSED.data());
  }
}

SCENARIO("Partial config of server") {
  SECTION("Without port") {
    std::vector<std::string> arguments = {"program_name", "--mode", "server",
                                          "--public-key", "public_key.pem",
                                          "--private-key", "private_key.pem"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_SERVER_CONFIG.data());
  }

  SECTION("Without public key") {
    std::vector<std::string> arguments = {"program_name", "--mode", "server",
                                          "--port", "3333",
                                          "--private-key", "private_key.pem"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_SERVER_CONFIG.data());
  }

  SECTION("Without private key") {
    std::vector<std::string> arguments = {"program_name", "--mode", "server",
                                          "--port", "3333",
                                          "--public-key", "public_key.pem"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::WRONG_SERVER_CONFIG.data());
  }

  SECTION("Without mode") {
    std::vector<std::string> arguments = {"program_name",
                                          "--port", "3333",
                                          "--public-key", "public_key.pem",
                                          "--private-key", "private_key.pem"};
    CHECK_THROWS_WITH(ParseFromVector(arguments), constants::ExceptionMessage::OPTION_MODE_IS_MISSED.data());
  }
}
