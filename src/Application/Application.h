#pragma once
#include <filesystem>

namespace app {

class Application {
 public:
  void Send(std::string_view ip, unsigned int port, std::filesystem::path file_path);
  std::filesystem::path Listen(unsigned int port, const std::string& public_rsa_key, const std::string& private_rsa_key);

 private:

};

} // namespace app
