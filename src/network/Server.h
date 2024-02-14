#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <string>
#include <optional>
#include <vector>

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/timer/progress_display.hpp>

#include "constants_storage.h"
#include "util/logger/logger.h"

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
  std::string Start(int port);
  void SendPublicKey(const std::string &public_key);
  fs::path DownloadFile();

 private:
  std::string Read();
  void Send(const std::vector<std::string_view> &response);
  std::pair<std::string, uint64_t> GetNameAndSize(const std::string &input);

 private:
  net::io_context io_context_;
  tcp::socket socket_{io_context_};
};

}  // namespace network
