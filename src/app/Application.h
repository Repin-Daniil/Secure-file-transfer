#pragma once

#include <filesystem>
#include <string_view>
#include <iostream>
#include <string>

#include "../Network/Server.h"
#include "../Network/Client.h"
#include "../Crypto/Crypto.h"
#include "../util/logger/logger.h"

namespace app {

namespace fs = std::filesystem;

using namespace std::literals;
/*!
 * @brief
 * @todo Если файлов несколько, то паковать в zip-архив
 * @todo Сделать broadcast отправку
 */
class Application {
 public:
  void Send(std::string_view ip, int port, fs::path file_path);
  void Listen(int port, const std::string &public_rsa_key, const std::string &private_rsa_key);
};

}  // namespace app
