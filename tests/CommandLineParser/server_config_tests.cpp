#include "test_framework_for_parser.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

std::unique_ptr<config::Config> GetServerConfig(int port, std::string &public_key_path, std::string &private_key_path);
void TestServer(int port, std::string &public_key_path, std::string &private_key_path);

SCENARIO("Server config", "[CommandLineParser]") {
  int port = 0;
  std::string public_key_path = "public_key.pem";
  std::string private_key_path = "private_key.pem";

  SECTION("Correct program options") {
    port = 3333;
    TestServer(port, public_key_path, private_key_path);
  }

  SECTION("Options with mistakes") {
    SECTION("Negative port number") {
      port = -3333;
      CHECK_THROWS_WITH(TestServer(port, public_key_path, private_key_path),
                        constants::ExceptionMessage::NON_EXISTENT_PORT.data());
    }SECTION("Non-existent port") {
      port = 10000000;
      CHECK_THROWS_WITH(TestServer(port, public_key_path, private_key_path),
                        constants::ExceptionMessage::NON_EXISTENT_PORT.data());
    }
  }
}

std::unique_ptr<config::Config> GetServerConfig(int port, std::string &public_key_path, std::string &private_key_path) {
  using namespace constants;
  std::vector<std::string> arguments = {"program_name"s, "--mode"s, "server",
                                        "--port"s, std::to_string(port),
                                        "--public-key"s, public_key_path,
                                        "--private-key"s, private_key_path};

  return ParseFromVector(arguments);
}

void TestServer(int port, std::string &public_key_path, std::string &private_key_path) {
  auto config = GetServerConfig(port, public_key_path, private_key_path);
  auto &server_config = dynamic_cast<config::ServerConfig &>(*config);

  REQUIRE(server_config.mode == config::Mode::SERVER);
  CHECK(server_config.port == port);
  CHECK(server_config.public_key_path == public_key_path);
  CHECK(server_config.private_key_path == private_key_path);
}
