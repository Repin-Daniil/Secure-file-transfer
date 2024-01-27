#pragma once

#include <string_view>

namespace constants {

using namespace std::literals;

struct ExceptionMessage {
  ExceptionMessage() = delete;

  constexpr static std::string_view WRONG_SERVER_CONFIG = "Wrong configuration for server"sv;
  constexpr static std::string_view WRONG_CLIENT_CONFIG = "Wrong configuration for client"sv;
  constexpr static std::string_view OPTION_MODE_IS_MISSED = "Option mode is missed"sv;
  constexpr static std::string_view UNKNOWN_MODE = "Unknown mode. Choose server/client"sv;
  constexpr static std::string_view WRONG_ADDRESS_FORMAT = "Wrong address format"sv;
  constexpr static std::string_view NO_PACKAGES = "No packages"sv;
  constexpr static std::string_view NON_EXISTENT_PORT = "Non-existent port"sv;
  constexpr static std::string_view NON_EXISTENT_IP = "Non-existent IP"sv;
};

struct ProgramOptions {
  ProgramOptions() = delete;

  constexpr static std::string_view GENERAL_OPTIONS = "General options"sv;
  constexpr static std::string_view SERVER_OPTIONS = "Server options"sv;
  constexpr static std::string_view CLIENT_OPTIONS = "Client options"sv;

  // General options
  constexpr static std::string_view MODE = "mode"sv;
  constexpr static std::string_view MODE_SERVER = "server"sv;
  constexpr static std::string_view MODE_CLIENT = "client"sv;
  constexpr static std::string_view MODE_VALUE_NAME = "client/server"sv;
  constexpr static std::string_view MODE_DESC = "Set mode: server/client"sv;

  constexpr static std::string_view HELP = "help"sv;
  constexpr static std::string_view HELP_DESC = "produce help message"sv;

  constexpr static std::string_view CONFIG = "config"sv;
  constexpr static std::string_view CONFIG_VALUE_NAME = "path"sv;
  constexpr static std::string_view CONFIG_DESC = "Path to config in JSON"sv;

  // Server options
  constexpr static std::string_view PORT = "port"sv;
  constexpr static std::string_view PORT_VALUE_NAME = "port number"sv;
  constexpr static std::string_view PORT_DESC = "Set listening port: from 1024 до 49151 without root"sv;

  constexpr static std::string_view PUBLIC_KEY = "public-key"sv;
  constexpr static std::string_view PUBLIC_KEY_VALUE_NAME = "path"sv;
  constexpr static std::string_view PUBLIC_KEY_DESC = "Path to RSA Public Key (public_key.pem)"sv;

  constexpr static std::string_view PRIVATE_KEY = "private-key"sv;
  constexpr static std::string_view PRIVATE_KEY_VALUE_NAME = "path"sv;
  constexpr static std::string_view PRIVATE_KEY_DESC = "Path to RSA Private Key (private_key.pem)"sv;

  // Client options
  constexpr static std::string_view ADDRESSES = "addresses"sv;
  constexpr static std::string_view ADDRESSES_VALUE_NAME = "{ip/port}"sv;
  constexpr static std::string_view ADDRESSES_DESC = "Receiver's addresses"sv;
  constexpr static std::string_view ADDRESSES_DELIMITER = "/"sv;

  constexpr static std::string_view PACKAGES = "packages"sv;
  constexpr static std::string_view PACKAGES_VALUE_NAME = "path"sv;
  constexpr static std::string_view PACKAGES_DESC = "Set path to files"sv;
};

} // namespace constants