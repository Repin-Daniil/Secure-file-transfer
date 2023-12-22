#include <string_view>
#include <filesystem>
#include <iostream>

#include "Application.h"
#include "../Network/Server.h"
#include "../Network/Client.h"
#include "../Crypto/Crypto.h"

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
  crypto::Crypto crypto(public_key);

  auto encrypted_file_path = crypto.EncryptFile(file_path);

  client.SendFile({encrypted_file_path.filename(), fs::file_size(encrypted_file_path),
                   {encrypted_file_path, std::ios::binary}});
}

void Application::Listen(unsigned int port,
                         const std::string &public_rsa_key,
                         const std::string &private_rsa_key) {
  network::Server server;
  crypto::Crypto crypto(public_rsa_key, private_rsa_key);

  server.Start(port);

  auto public_key = crypto.getPublicKeyAsPEM();
  server.SendPublicKey(public_key);

  auto encrypted_file_path = server.DownloadFile();
  std::string decrypted_file_name = crypto.DecryptFile(encrypted_file_path);
}

} // namespace app