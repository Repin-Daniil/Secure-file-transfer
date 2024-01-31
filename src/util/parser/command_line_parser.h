#pragma once

#include <memory>
#include <utility>
#include <optional>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/asio/ip/address.hpp>

#include "../../config.h"
#include "../../constants_storage.h"
#include "../logger/logger.h"

namespace util {

namespace fs = std::filesystem;

using namespace std::literals;

/*!
 * @brief Парсер командной строки
 * @todo Добавить JSON конфиг, тогда надо будет разделить на ConfigLoader, JsonLoader, CommandLineParser
 * @todo  Добавить параметр --broadcast для отправки файлов всем сразу
 * @todo  Посылки нужно обрамлять в фигурные скобки
 */
class CommandLineParser {
 public:
  std::unique_ptr<config::Config> ParseCommandLine(int argc, const char *const argv[]);

 private:
  std::vector<config::ClientConfig::Address> ParseAddresses();
  std::vector<fs::path> ParsePaths();
  config::ClientConfig::Address ConvertStrToAddress(const std::string &address_str);

 private:
  struct Args {
    std::string mode;
    fs::path path_to_config;

    // Server args
    int port;
    std::string path_to_public_key;
    std::string path_to_private_key;

    // Client args
    std::vector<std::string> addresses;
    std::vector<std::string> path_to_packages;
  };

  Args args_;
};

}  // namespace util
