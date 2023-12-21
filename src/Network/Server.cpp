#include <iostream>
#include "Server.h"
#include <filesystem>
#include <fstream>

namespace network {

void Server::Start() {
  tcp::acceptor acceptor(io_context_, tcp::endpoint(tcp::v4(), port_));
  std::cout << "Waiting for connection"sv << std::endl;
  boost::system::error_code ec;
  acceptor.accept(socket_, ec);

  if (ec) {
    throw std::runtime_error("Can`t accept connection");
  }

  net::streambuf stream_buf;
  net::read_until(socket_, stream_buf, '\n', ec);
  std::string client_data{std::istreambuf_iterator<char>(&stream_buf), std::istreambuf_iterator<char>()};

  if (ec) {
    throw std::runtime_error("Error reading data");
  }

  std::cout << "Get Request for " + client_data;
}

void Server::SendPublicKey(std::string public_key) {
  boost::system::error_code ec;
  socket_.write_some(net::buffer(public_key + "\n"), ec);

  if (ec) {
    throw std::runtime_error("Error while sending public key");
  }

  std::cout << "Send Public key to client" << std::endl;
}

void Server::GetFile() {
  boost::system::error_code ec;

  net::streambuf stream_buf;
  net::read_until(socket_, stream_buf, '\n', ec);
  std::string file_name{std::istreambuf_iterator<char>(&stream_buf), std::istreambuf_iterator<char>()};

  if (ec) {
    throw std::runtime_error("Error while reading file name");
  }

  net::read_until(socket_, stream_buf, '\n', ec);
  std::string file_size_str{std::istreambuf_iterator<char>(&stream_buf), std::istreambuf_iterator<char>()};
  uint64_t file_size = std::atoi(file_size_str.data());

  if (ec) {
    throw std::runtime_error("Error while reading file size");
  }

  std::cout << "File name: " << file_name;
  std::cout << "File size: " << file_size << std::endl;

  //TODO Send file
}

} // namespace network