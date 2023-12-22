#include <string_view>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Application.h"
#include "../Network/Server.h"
#include "../Network/Client.h"

namespace app {

namespace fs = std::filesystem;
using namespace fs;

void Application::Send(std::string_view server_ip,
                       unsigned int port,
                       std::filesystem::path file_path) {

  if (!fs::exists(file_path)) {
    throw std::runtime_error("File not found!");
  }

  network::Client client;
  client.Connect(server_ip, port);
  auto public_key = client.RequestServerPublicKey();
  //TODO шифруем файл
  client.SendFile({file_path.filename(), fs::file_size(file_path), {file_path, std::ios::binary}});
  //TODO Удаляем зашифрованный файл
}

std::filesystem::path Application::Listen(unsigned int port, std::string public_rsa_key, std::string private_rsa_key) {
  network::Server server(port);
  server.Start();
  server.SendPublicKey(public_rsa_key);
  server.GetFile();

  //TODO Расшифровать

  return fs::current_path();
}

} // namespace app