#pragma once

#include <vector>
#include <string>
#include <utility>
#include <filesystem>

namespace config {

namespace fs = std::filesystem;

enum Mode { NONE, CLIENT, SERVER };

struct Config {
  explicit Config(Mode user_choice) : mode(user_choice) {}
  virtual ~Config() = default;

  Mode mode;
};

struct ServerConfig : Config {
  ServerConfig(int listening_port, fs::path pub_key, fs::path private_key)
      : Config(Mode::SERVER),
        port(listening_port),
        public_key_path(std::move(pub_key)),
        private_key_path(std::move(private_key)) {
  }

  int port;
  fs::path public_key_path;
  fs::path private_key_path;
};

struct ClientConfig : Config {
  using Address = std::pair<std::string, int>;

  ClientConfig(std::vector<Address> addresses, std::vector<fs::path> packages)
      : Config(Mode::CLIENT),
        servers(std::move(addresses)),
        path_to_packages(std::move(packages)) {
  }

  std::vector<Address> servers;
  std::vector<fs::path> path_to_packages;
};

struct NullConfig : Config {
  NullConfig() : Config(Mode::NONE) {}
};

}  // namespace config
