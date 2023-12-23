#pragma once

#include <filesystem>
#include <string_view>
#include <iostream>

#include "../Network/Server.h"
#include "../Network/Client.h"
#include "../Crypto/Crypto.h"

namespace app {

namespace fs = std::filesystem;

class Application {
 public:
  void Send(std::string_view ip, unsigned int port, fs::path file_path);
  void Listen(unsigned int port, const std::string &public_rsa_key, const std::string &private_rsa_key);
};

} // namespace app
