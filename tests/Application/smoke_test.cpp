#include <gtest/gtest.h>
#include <fstream>

#include "../../src/config.h"
#include "../../src/app/Application.h"

namespace {

namespace fs = std::filesystem;

struct ScopedLogDisable {
  ScopedLogDisable() {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::fatal);
  }

  ~ScopedLogDisable() {
    boost::log::core::get()->reset_filter();
  }
};

}  // namespace

bool AreFilesEqual(const fs::path file_1, const fs::path &file_2) {
  std::ifstream stream_1(file_1, std::ios::binary);
  std::ifstream stream_2(file_2, std::ios::binary);

  if (!stream_1 || !stream_2) {
    return false;
  }

  char symbol_1, symbol_2;

  while (stream_1.get(symbol_1) && stream_2.get(symbol_2)) {
    if (symbol_1 != symbol_2) {
      return false;
    }
  }

  return true;
}

TEST(SafeFileTransfer, SmokeTest) {
  // Arrange
  ScopedLogDisable disable_logger;

  app::Application server;
  app::Application client;

  fs::path server_package;
  fs::path client_package = "../res/package.txt";
  fs::path public_key = "../res/public_key.pem";
  fs::path private_key = "../res/private_key.pem";

  int port = 3333;
  std::string localhost = "127.0.0.1";

  config::ServerConfig server_config{port, public_key, private_key};
  config::ClientConfig client_config{{config::ClientConfig::Address(localhost, port)}, {client_package}};

  // Act
  std::thread server_thread([&]() {
    server_package = server.Listen(server_config.port, server_config.public_key_path, server_config.private_key_path);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  client.Send(client_config.servers.at(0).first,
              client_config.servers.at(0).second,
              client_config.path_to_packages.at(0));

  server_thread.join();

  // Assert
  ASSERT_EQ(fs::file_size(server_package), fs::file_size(client_package));
  ASSERT_EQ(server_package.filename(), client_package.filename());
  ASSERT_TRUE(AreFilesEqual(server_package, client_package));
}
