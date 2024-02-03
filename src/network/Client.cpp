#include "Client.h"

namespace network {

using constants::NetworkConstants;

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
  Send({NetworkConstants::REQUEST_FOR_PUBLIC_KEY});

  LogTrace("Read Public Key"sv);
  auto public_key = GetPublicKeyFromResponse(Read());

  return public_key;
}

void Client::SendFile(Package package) {
  boost::system::error_code ec;

  std::stringstream file_name;
  std::stringstream file_size;

  file_name << NetworkConstants::FILE_NAME_HEADER << package.file_name;
  file_size << NetworkConstants::FILE_SIZE_HEADER << std::to_string(package.file_size);

  LogTrace("Send file name and size"sv);
  Send({file_name.str(), file_size.str()});

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
  auto response = Read();

  LogInfo(response);
}

std::string Client::GetPublicKeyFromResponse(const std::string &response) {
  boost::regex pattern(NetworkConstants::PUBLIC_KEY_PATTERN.data());
  boost::smatch matches;

  if (boost::regex_search(response, matches, pattern)) {
    if (matches.size() > 1) {
      return matches[1];
    }
  }

  throw std::runtime_error("Wrong Format. No public key");
}

std::string Client::Read() {
  boost::system::error_code ec;
  net::streambuf stream_buf;

  net::read_until(socket_, stream_buf, NetworkConstants::DOUBLE_CRLF, ec);

  if (ec) {
    throw std::runtime_error("Error while reading response");
  }

  std::string request{std::istreambuf_iterator<char>(&stream_buf),
                      std::istreambuf_iterator<char>()};

  LogTrace("Response: "s + request);

  return request;
}

void Client::Send(const std::vector<std::string_view> &response) {
  std::stringstream ss;
  boost::system::error_code ec;

  for (auto &msg : response) {
    ss << msg << NetworkConstants::CRLF;
  }

  ss << NetworkConstants::CRLF;
  LogTrace("Request: "s + ss.str());

  socket_.write_some(net::buffer(ss.str()), ec);

  if (ec) {
    throw std::runtime_error("Error while sending request");
  }
}

}  // namespace network
