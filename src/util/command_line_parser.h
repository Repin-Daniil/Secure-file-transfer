#pragma once

#include <memory>
#include <utility>
#include <optional>
#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/asio/ip/address.hpp>

#include "../constants_storage.h"

namespace util {

namespace fs = std::filesystem;

using namespace std::literals;

enum Mode { None, Client, Server };

struct Config {
  Config(Mode user_choice) : mode(user_choice) {}
  virtual ~Config()= default;

  Mode mode;
};

struct ServerConfig : Config {
  ServerConfig(unsigned int listening_port, fs::path pub_key, fs::path private_key)
      : Config(Mode::Server),
        port(listening_port),
        public_key_path(std::move(pub_key)),
        private_key_path(std::move(private_key)) {
  }

  unsigned int port;
  fs::path public_key_path;
  fs::path private_key_path;
};

struct ClientConfig : Config {
  using Address = std::pair<std::string, unsigned int>;

  ClientConfig(std::vector<Address> addresses, std::vector<fs::path> packages)
      : Config(Mode::Client),
        servers(std::move(addresses)),
        path_to_packages(std::move(packages)) {
  }

  std::vector<Address> servers;
  std::vector<fs::path> path_to_packages;
};

struct NullConfig : Config {
  NullConfig() : Config(Mode::None) {}
};

//TODO Подгружать бы конфиг из JSON, тогда надо будет разделить на ConfigLoader, JsonLoader, CommandLineParser
class CommandLineParser {
 public:
  std::unique_ptr<Config> ParseCommandLine(int argc, const char *const argv[]);

 private:
  std::vector<ClientConfig::Address> ParseAddresses();
  std::vector<fs::path> ParsePaths();
  ClientConfig::Address ConvertStrToAddress(const std::string &address_str);

 private:
  struct Args {
    std::string mode;
    fs::path path_to_config;

    // Server args
    unsigned int port;
    std::string path_to_public_key;
    std::string path_to_private_key;

    // Client args
    std::vector<std::string> addresses;
    std::vector<std::string> path_to_packages;
  };

  Args args_;
};

} // namespace util