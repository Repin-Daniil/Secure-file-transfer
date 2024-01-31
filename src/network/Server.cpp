#include "Server.h"

namespace network {

void Server::Start(int port) {
  boost::system::error_code ec;

  std::filesystem::create_directory(std::filesystem::current_path().c_str() + "/tmp"s);
  LogInfo("Server start"sv);
  LogInfo("Waiting for connection"sv);

  tcp::acceptor acceptor(io_context_, tcp::endpoint(tcp::v4(), port));
  acceptor.accept(socket_, ec);

  if (ec) {
    throw std::runtime_error("Can`t accept connection");
  }

  LogInfo("Accept connection"sv);

  auto request = ReadFromSocket();

  LogTrace(request);
}

void Server::SendPublicKey(const std::string &public_key) {
  boost::system::error_code ec;
  socket_.write_some(net::buffer(public_key + "\n"), ec);

  if (ec) {
    throw std::runtime_error("Error while sending public key");
  }

  LogTrace("Send public key"sv);
}

fs::path Server::DownloadFile() {
  auto file_data = GetNameAndSize(ReadFromSocket());

  std::string file_name = file_data.first;
  uint64_t file_size = file_data.second;

  LogTrace("File name: "s + file_name);
  LogTrace("File size: " + std::to_string(file_size));

  auto path = fs::current_path().c_str() + "/tmp/"s + file_name;
  std::ofstream output_file_stream(path, std::ios::binary);

  if (!output_file_stream) {
    throw std::runtime_error("Can't create file");
  }

  LogInfo("Download started"sv);

  net::streambuf buffer;
  boost::timer::progress_display progress_bar(file_size);
  auto start = std::chrono::steady_clock::now();

  if (buffer.size()) {
    progress_bar += buffer.size();
    output_file_stream << &buffer;
  }

  while (progress_bar.count() < file_size) {
    progress_bar += boost::asio::read(socket_, buffer, boost::asio::transfer_at_least(1));
    output_file_stream << &buffer;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  LogInfo("Download complete"sv);
  LogTrace("Received bytes: "s + std::to_string(progress_bar.count()));
  LogTrace("Loading time: "s + std::to_string(elapsed_seconds.count()) + " seconds"s);

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
