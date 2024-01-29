#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <boost/asio.hpp>

namespace network {

namespace net = boost::asio;
namespace ip = net::ip;

using ip::tcp;
using namespace std::literals;

struct Package {
  std::string file_name;
  uint64_t file_size;
  std::ifstream stream;
};

/*!
 * @brief Класс для отправки файлов по TCP
 * @todo Ну разным серверам можно в разных потоках отправлять
 */
class Client {
 public:
  void Connect(std::string_view ip_address, int port);
  std::string RequestServerPublicKey();
  void SendFile(Package package);

 private:
  net::io_context io_context_;
  tcp::socket socket_{io_context_};

  // Methods
  std::string ReadFromSocket();
};

}  // namespace network
