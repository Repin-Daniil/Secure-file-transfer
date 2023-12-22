#include <iostream>
#include "Server.h"
#include <boost/regex.hpp>

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
  net::read_until(socket_, stream_buf, "\n", ec);

  if (ec) {
    throw std::runtime_error("Error while reading file name and file size");
  }

  std::string file_data{std::istreambuf_iterator<char>(&stream_buf), std::istreambuf_iterator<char>()};

  std::string file_name;
  uint64_t file_size;

  boost::smatch matches;
  boost::regex pattern("FileName\\(([^)]+)\\);\\sFileSize\\((\\d+)\\)");

  if (boost::regex_search(file_data, matches, pattern)) {
    file_name = matches[1];
    file_size = std::atoi(matches[2].str().data());
  } else {
    throw std::runtime_error("Error, Wrong Format!");
  }

  std::cout << "File name: " << file_name<< " File size: " << file_size << std::endl;

  std::ofstream output_file_stream(fs::current_path().c_str() + "/"s + file_name, std::ios::binary);

  if(!output_file_stream){
    throw std::runtime_error("Can't create file");
  }

  std::size_t bytes_amount = 0;
  boost::asio::streambuf buffer;

  std::cout << "Buffer size = " << buffer.size() << std::endl;

  if(buffer.size()){
    bytes_amount += buffer.size();
    output_file_stream << &buffer;
  }

  while(bytes_amount < file_size){
    bytes_amount += boost::asio::read(socket_, buffer, boost::asio::transfer_at_least(1));
    output_file_stream << &buffer;
    std::cout << "Bytes recieved " << bytes_amount << std::endl;
  }
}

} // namespace network