#pragma once

#include <filesystem>
#include <string_view>
#include <iostream>
#include <string>

#include "../network/Server.h"
#include "../network/Client.h"
#include "../crypto/Crypto.h"
#include "../util/logger/logger.h"

namespace app {

namespace fs = std::filesystem;

using namespace std::literals;

/*!
 * @brief
 * @todo Если файлов несколько, то паковать в zip-архив
 * @todo Добавить контрольную сумму
 * @todo Сделать broadcast отправку
 */
class Application {
 public:
  void Send(std::string_view ip, int port, fs::path file_path);
  fs::path Listen(int port, const std::string &public_rsa_key, const std::string &private_rsa_key);
};

}  // namespace app
