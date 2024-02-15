#include "Application.h"

namespace app {

using constants::NetworkConstants;
using constants::LogTag;
using logger::LogInfo;

void Application::Send(std::string_view server_ip,
                       int port,
                       fs::path file_path) {
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

fs::path Application::Listen(int port,
                         const std::string &public_rsa_key,
                         const std::string &private_rsa_key) {
  network::Server server;
  crypto::Crypto crypto(public_rsa_key, private_rsa_key);

  auto request = server.Start(port);

  auto public_key = crypto.GetPublicKeyAsString();
  server.SendPublicKey(public_key);

  auto encrypted_file_path = server.DownloadFile();
  std::string decrypted_file_name = crypto.DecryptFile(encrypted_file_path);

  LogInfo(LogTag::SERVER, "Save file to "s + decrypted_file_name);

  return decrypted_file_name;
}

}  // namespace app
