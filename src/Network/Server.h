#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <string>

#include <boost/asio.hpp>
#include <boost/regex.hpp>

namespace network {

namespace net = boost::asio;
namespace fs = std::filesystem;
namespace ip = net::ip;

using ip::tcp;
using namespace std::literals;

/*!
 * @brief Сервер для приема файлов
 * @todo перевести логику обработки из app в Server. Больше асинхронщины
 */
class Server {
 public:
  void Start(int port);
  void SendPublicKey(const std::string &public_key);
  fs::path DownloadFile();

 private:
  net::io_context io_context_;
  tcp::socket socket_{io_context_};

  //  Methods
  std::string ReadFromSocket();
  std::pair<std::string, uint64_t> GetNameAndSize(const std::string &input);
};

}  // namespace network
