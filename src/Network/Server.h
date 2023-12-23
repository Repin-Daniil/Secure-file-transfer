#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/asio.hpp>
#include <boost/regex.hpp>

namespace network {

namespace net = boost::asio;
namespace fs = std::filesystem;

using net::ip::tcp;
using namespace std::literals;

class Server {
 public:
  void Start(unsigned int port);
  void SendPublicKey(const std::string &public_key);
  fs::path DownloadFile();

 private:
  net::io_context io_context_;
  tcp::socket socket_{io_context_};

  // Methods
  std::string ReadFromSocket();
  std::pair<std::string, uint64_t> GetNameAndSize(const std::string &input);
};

} // namespace network
