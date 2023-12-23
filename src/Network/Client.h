#pragma once

#include <boost/asio.hpp>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace network {

namespace net = boost::asio;

using net::ip::tcp;
using namespace std::literals;

struct Package {
  std::string file_name;
  uint64_t file_size;
  std::ifstream stream;
};

class Client {
 public:
  void Connect(std::string_view ip_address, unsigned int port);
  std::string RequestServerPublicKey();
  void SendFile(Package package);

 private:
  net::io_context io_context_;
  tcp::socket socket_{io_context_};

  // Methods
  std::string ReadFromSocket();
};

} // namespace network