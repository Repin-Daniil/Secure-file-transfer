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

  socket_.write_some(boost::asio::buffer(package.file_name + "\n"), ec);

  if (ec) {
    throw std::runtime_error("Error sending FileName");
  }

  socket_.write_some(boost::asio::buffer(std::to_string(package.file_size) + "\n"), ec);
  std::cout << "File name: " << package.file_name << "\nFile size: " << package.file_size << std::endl;

  if (ec) {
    throw std::runtime_error("Error sending FileSize");
  }

  //TODO
//  std::uintmax_t sentFileBody = 0;
//  char fileBuf[10000];
//  std::size_t fileBufSize = sizeof(fileBuf);
//
//  while(package.stream){
//    package.stream.read(fileBuf, fileBufSize);
//    sentFileBody += socket_.write_some(boost::asio::buffer(fileBuf, package.stream.gcount()), ec); // net::transfer_exactly
//  }
//
//  if(sentFileBody != package.file_size){
//    throw std::runtime_error("Package sending failed");
//  }
//
//  std::cout << "sentFileName = " << package.file_name << std::endl
//            << "sentFileSize = " << package.file_size << std::endl
//            << "sentFileBody = " << sentFileBody << std::endl;
//
//  package.stream.close();
}

} // namespace network