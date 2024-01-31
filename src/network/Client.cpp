#include "Client.h"

namespace network {

void Client::Connect(std::string_view ip_address, int port) {
  boost::system::error_code ec;
  auto endpoint = tcp::endpoint(net::ip::make_address(ip_address, ec), port);

  if (ec) {
    throw std::invalid_argument("Wrong IP Format");
  }

  socket_.connect(endpoint, ec);

  if (ec) {
    throw std::runtime_error("Can't connect to server");
  }

  LogInfo("Connect to server"sv);
}

std::string Client::RequestServerPublicKey() {
  boost::system::error_code ec;

  LogTrace("Request Public Key from server");
  socket_.write_some(net::buffer("Request for public key\n"sv), ec);

  if (ec) {
    throw std::runtime_error("Error sending request for public key");
  }

  auto public_key = ReadFromSocket();
  LogTrace("Get Public Key from server");

  return public_key;
}

void Client::SendFile(Package package) {
  boost::system::error_code ec;

  LogTrace("Send file name and size"sv);
  net::write(socket_,
             boost::asio::buffer(
                 "FileName(" + package.file_name + "); FileSize(" + std::to_string(package.file_size) + ")\n"),
             ec);

  if (ec) {
    throw std::runtime_error("Error sending FileName and FileSize");
  }

  LogInfo("Upload start"sv);

  boost::timer::progress_display progress_bar(package.file_size);
  auto start = std::chrono::steady_clock::now();

  std::array<char, 10000> file_buffer{};
  size_t buffer_size = file_buffer.size();

  while (package.stream) {
    package.stream.read(file_buffer.data(), buffer_size);
    progress_bar += net::write(socket_, net::buffer(file_buffer.data(), package.stream.gcount()), ec);
  }

  package.stream.close();

  LogTrace("Bytes sent: "s + std::to_string(progress_bar.count()));

  if (progress_bar.count() != package.file_size) {
    throw std::runtime_error("Package sending failed");
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  LogTrace("File Name: "s + package.file_name);
  LogTrace("File Size: "s + std::to_string(package.file_size));

  LogInfo("Waiting for server's response");
  LogInfo(ReadFromSocket());
}

std::string Client::ReadFromSocket() {
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
