#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/timer/progress_display.hpp>

#include "constants_storage.h"
#include "util/logger/logger.h"

namespace network {

namespace net = boost::asio;
namespace ip = net::ip;

using namespace std::literals;

using ip::tcp;
using constants::LogTag;
using logger::LogInfo;
using logger::LogTrace;
using constants::ExceptionMessage;
using constants::NetworkConstants;

struct Package {
  std::string file_name;
  uint64_t file_size;
  std::ifstream stream;
};

class Client {
 public:
  void Connect(std::string_view ip_address, int port);
  std::string RequestServerPublicKey();
  void SendFile(Package package);

 private:
  std::string Read();
  void Send(const std::vector<std::string_view> &response);
  std::string GetPublicKeyFromResponse(const std::string &response);

 private:
  net::io_context io_context_;
  tcp::socket socket_{io_context_};
};

}  // namespace network
