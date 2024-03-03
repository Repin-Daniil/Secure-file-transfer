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
  constexpr static std::string_view NO_CONNECTION = "Can't connect to server"sv;
  constexpr static std::string_view NO_ACCEPT = "Can't accept connection"sv;
  constexpr static std::string_view NO_PUBLIC_KEY = "Can't parse public key from response"sv;
  constexpr static std::string_view READING_ERROR = "Error while reading"sv;
  constexpr static std::string_view SENDING_ERROR = "Error while sending"sv;
  constexpr static std::string_view FILE_CREATION_ERROR = "Can't create file"sv;
  constexpr static std::string_view NO_FILE_ATTRIBUTES = "Can't parse file attributes!"sv;
  constexpr static std::string_view NO_FILE = "File not found!"sv;
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

struct NetworkConstants {
  NetworkConstants() = delete;

  constexpr static int kFirstPort = 0;
  constexpr static int kLastPort = 65535;

  constexpr static std::string_view CRLF = "\r\n"sv;
  constexpr static std::string_view DOUBLE_CRLF = "\r\n\r\n"sv;

  constexpr static std::string_view REQUEST_FOR_PUBLIC_KEY = "RPK"sv;

  constexpr static std::string_view FILE_NAME_HEADER = "File name: "sv;
  constexpr static std::string_view FILE_SIZE_HEADER = "File size: "sv;
  constexpr static std::string_view PUBLIC_KEY_HEADER = "Public key: "sv;

  constexpr static std::string_view PUBLIC_KEY_PATTERN = "Public key: (.+)";
  constexpr static std::string_view FILE_ATTRIBUTES_PATTERN = "File name: (.+)\r\nFile size: (\\d+)";

  constexpr static std::string_view OK = "OK"sv;
  constexpr static std::string_view FAILED = "FAILED"sv;
};

struct LogTag {
  LogTag() = delete;

  constexpr static std::string_view SERVER = "[SERVER]"sv;
  constexpr static std::string_view CLIENT = "[CLIENT]"sv;
};
}  // namespace constants
