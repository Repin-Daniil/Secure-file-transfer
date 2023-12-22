#include <iostream>
#include "Client.h"

namespace network {

void Client::Connect(std::string_view ip_address, unsigned int port) {
  boost::system::error_code ec;
  auto endpoint = tcp::endpoint(net::ip::make_address(ip_address, ec), port);

  if (ec) {
    throw std::invalid_argument("Wrong IP Format");
  }

  socket_.connect(endpoint, ec);

  if (ec) {
    throw std::runtime_error("Can't connect to server");
  }

  std::cout << "Connect to server" << std::endl;
}

std::string Client::RequestServerPublicKey() {
  boost::system::error_code ec;

  socket_.write_some(net::buffer("Public key\n"sv), ec);

  if (ec) {
    throw std::runtime_error("Error sending request for publc key");
  }

  net::streambuf stream_buf;
  net::read_until(socket_, stream_buf, '\n', ec);

  std::string public_key{std::istreambuf_iterator<char>(&stream_buf),
                         std::istreambuf_iterator<char>()};

  if (ec) {
    throw std::runtime_error("Error reading Public key");
  }

  std::cout << "Get Public Key" << std::endl;

  return public_key;
}

void Client::SendFile(Package package) {
  boost::system::error_code ec;

  net::write(socket_,
             boost::asio::buffer(
                 "FileName(" + package.file_name + "); FileSize(" + std::to_string(package.file_size) + ")\n"),
             ec);

  if (ec) {
    throw std::runtime_error("Error sending FileName and FileSize");
  }


  std::array<char, 10000> file_buffer;
  std::uint64_t bytes_sent = 0;
  std::size_t buffer_size = file_buffer.size();

  while (package.stream) {
    package.stream.read(file_buffer.data(), buffer_size);
    bytes_sent += net::write(socket_, boost::asio::buffer(file_buffer.data(), package.stream.gcount()), ec);
  }

  if (bytes_sent != package.file_size) {
    throw std::runtime_error("Package sending failed");
  }

  std::cout << "File Name: " << package.file_name << std::endl
            << "File Size: " << package.file_size << std::endl
            << "Bytes sent: " << bytes_sent << std::endl;

  package.stream.close();
}

} // namespace network