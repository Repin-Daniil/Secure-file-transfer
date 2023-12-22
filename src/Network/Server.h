#pragma once
#include <boost/asio.hpp>
#include <filesystem>

namespace network {

namespace net = boost::asio;
namespace fs = std::filesystem;

using net::ip::tcp;
using namespace fs;
using namespace std::literals;

class Server {
 public:
  Server(unsigned int port) : port_(port) {
  }

  void Start();
  void SendPublicKey(std::string public_key);
  void GetFile();

 private:
  unsigned int port_;
  net::io_context io_context_;
  tcp::socket socket_{io_context_};

};

} // namespace network
