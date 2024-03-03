#include "Server.h"

namespace network {

std::string Server::Start(int port) {
  boost::system::error_code ec;

  std::filesystem::create_directory(std::filesystem::current_path().c_str() + "/tmp"s);
  LogInfo(LogTag::SERVER, "Server start"sv);
  LogInfo(LogTag::SERVER, "Waiting for connection"sv);

  tcp::acceptor acceptor(io_context_, tcp::endpoint(tcp::v4(), port));
  acceptor.accept(socket_, ec);

  if (ec) {
    throw std::runtime_error(ExceptionMessage::NO_ACCEPT.data());
  }

  LogInfo(LogTag::SERVER, "Accept connection"sv);

  LogTrace(LogTag::SERVER, "Read Client Request"sv);
  auto request = Read();
  LogTrace(LogTag::SERVER, request);

  return request;
}

void Server::SendPublicKey(const std::string &public_key) {
  boost::system::error_code ec;

  LogTrace(LogTag::SERVER, "Send Public key to client");
  Send({NetworkConstants::PUBLIC_KEY_HEADER, public_key});
}

fs::path Server::DownloadFile() {
  boost::system::error_code ec;

  LogTrace(LogTag::SERVER, "Reading file attributes"sv);
  auto request = Read();
  auto file_data = GetNameAndSize(request);

  std::string file_name = file_data.first;
  uint64_t file_size = file_data.second;

  LogTrace(LogTag::SERVER, "File name: "s + file_name);
  LogTrace(LogTag::SERVER, "File size: "s + std::to_string(file_size));

  auto path = fs::current_path().c_str() + "/tmp/"s + file_name;
  std::ofstream output_file_stream(path, std::ios::binary);

  if (!output_file_stream) {
    throw std::runtime_error(ExceptionMessage::FILE_CREATION_ERROR.data());
  }

  LogInfo(LogTag::SERVER, "Download started"sv);

  net::streambuf buffer;
  boost::timer::progress_display progress_bar(file_size);
  auto start = std::chrono::steady_clock::now();

  if (buffer.size()) {
    progress_bar += buffer.size();
    output_file_stream << &buffer;
  }

  while (progress_bar.count() < file_size) {
    progress_bar += net::read(socket_, buffer, boost::asio::transfer_at_least(1));
    output_file_stream << &buffer;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  LogInfo(LogTag::SERVER, "Download complete"sv);
  LogTrace(LogTag::SERVER, "Received bytes: "s + std::to_string(progress_bar.count()));
  LogTrace(LogTag::SERVER, "Loading time: "s + std::to_string(elapsed_seconds.count()) + " seconds"s);

  LogTrace(LogTag::SERVER, "Send status to client");
  Send({NetworkConstants::OK});

  return path;
}

std::pair<std::string, uint64_t> Server::GetNameAndSize(const std::string &input) {
  boost::smatch matches;
  boost::regex pattern(NetworkConstants::FILE_ATTRIBUTES_PATTERN.data());

  if (boost::regex_search(input, matches, pattern)) {
    if (matches.size() > 2) {
      return std::pair(matches[1], std::stoll(matches[2]));
    }
  }

  throw std::runtime_error(ExceptionMessage::NO_FILE_ATTRIBUTES.data());
}

std::string Server::Read() {
  net::streambuf stream_buf;
  boost::system::error_code ec;

  net::read_until(socket_, stream_buf, NetworkConstants::DOUBLE_CRLF, ec);

  if (ec) {
    throw std::runtime_error(ExceptionMessage::READING_ERROR.data());
  }

  std::string request{std::istreambuf_iterator<char>(&stream_buf),
                      std::istreambuf_iterator<char>()};

  LogTrace(LogTag::SERVER, "Request: "s + request);

  return request;
}

void Server::Send(const std::vector<std::string_view> &response) {
  std::stringstream ss;
  boost::system::error_code ec;

  for (auto &msg : response) {
    ss << msg << NetworkConstants::CRLF;
  }

  ss << NetworkConstants::CRLF;
  LogTrace(LogTag::SERVER, "Response: "s + ss.str());

  socket_.write_some(net::buffer(ss.str()), ec);

  if (ec) {
    throw std::runtime_error(ExceptionMessage::SENDING_ERROR.data());
  }
}
}  // namespace network
