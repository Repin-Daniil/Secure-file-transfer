#include "Server.h"

namespace network {

void Server::Start(int port) {
  boost::system::error_code ec;

  std::filesystem::create_directory(std::filesystem::current_path().c_str() + "/tmp"s);

  std::cout << "Waiting for connection"sv << std::endl;

  tcp::acceptor acceptor(io_context_, tcp::endpoint(tcp::v4(), port));
  acceptor.accept(socket_, ec);

  if (ec) {
    throw std::runtime_error("Can`t accept connection");
  }

  std::cout << ReadFromSocket();
}

void Server::SendPublicKey(const std::string &public_key) {
  boost::system::error_code ec;
  socket_.write_some(net::buffer(public_key + "\n"), ec);

  if (ec) {
    throw std::runtime_error("Error while sending public key");
  }

  std::cout << "Send public key" << std::endl;
}

fs::path Server::DownloadFile() {
  auto file_data = GetNameAndSize(ReadFromSocket());

  std::string file_name = file_data.first;
  uint64_t file_size = file_data.second;

  std::cout << "File name: " << file_name << std::endl;
  std::cout << "File size: " << file_size << std::endl;

  auto path = fs::current_path().c_str() + "/tmp/"s + file_name;
  std::ofstream output_file_stream(path, std::ios::binary);

  if (!output_file_stream) {
    throw std::runtime_error("Can't create file");
  }

  std::cout << "Download started " << std::endl;

  size_t bytes_amount = 0;
  net::streambuf buffer;

  if (buffer.size()) {
    bytes_amount += buffer.size();
    output_file_stream << &buffer;
  }

  while (bytes_amount < file_size) {
    bytes_amount += boost::asio::read(socket_, buffer, boost::asio::transfer_at_least(1));
    output_file_stream << &buffer;
  }

  std::cout << "Download complete" << std::endl;
  std::cout << "Received bytes: " << bytes_amount << std::endl;

  socket_.write_some(net::buffer("Success\n"));

  return path;
}

std::pair<std::string, uint64_t> Server::GetNameAndSize(const std::string &input) {
  boost::smatch matches;
  boost::regex pattern(R"(FileName\(([^)]+)\);\sFileSize\((\d+)\))");

  if (boost::regex_search(input, matches, pattern)) {
    return std::pair(matches[1], std::stoll(matches[2].str()));
  }

  throw std::runtime_error("Error, Wrong Format!");
}

std::string Server::ReadFromSocket() {
  boost::system::error_code ec;

  net::streambuf stream_buf;
  net::read_until(socket_, stream_buf, '\n', ec);

  if (ec) {
    throw std::runtime_error("Error while reading");
  }

  std::string response{std::istreambuf_iterator<char>(&stream_buf),
                       std::istreambuf_iterator<char>()};

  return response;
}

}  // namespace network
