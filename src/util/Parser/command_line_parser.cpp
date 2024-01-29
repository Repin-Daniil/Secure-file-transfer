#include "command_line_parser.h"

namespace util {

std::unique_ptr<config::Config> CommandLineParser::ParseCommandLine(int argc, const char *const argv[]) {
  using constants::ProgramOptions;
  using constants::ExceptionMessage;
  namespace po = boost::program_options;

  po::options_description desc(ProgramOptions::GENERAL_OPTIONS.data());
  po::options_description server_options_description(ProgramOptions::SERVER_OPTIONS.data());
  po::options_description client_options_description(ProgramOptions::CLIENT_OPTIONS.data());

  desc.add_options()
      (ProgramOptions::MODE.data(),
       po::value(&args_.mode)->value_name(ProgramOptions::MODE_VALUE_NAME.data()),
       ProgramOptions::MODE_DESC.data())
      (ProgramOptions::HELP.data(), ProgramOptions::HELP_DESC.data())
      (ProgramOptions::CONFIG.data(),
       po::value(&args_.path_to_config)->value_name(ProgramOptions::CONFIG_VALUE_NAME.data()),
       ProgramOptions::CONFIG_DESC.data());

  server_options_description.add_options()
      (ProgramOptions::PORT.data(),
       po::value(&args_.port)->value_name(ProgramOptions::PORT_VALUE_NAME.data()),
       ProgramOptions::PORT_DESC.data())
      (ProgramOptions::PUBLIC_KEY.data(),
       po::value(&args_.path_to_public_key)->value_name(ProgramOptions::PUBLIC_KEY_VALUE_NAME.data()),
       ProgramOptions::PUBLIC_KEY_DESC.data())
      (ProgramOptions::PRIVATE_KEY.data(),
       po::value(&args_.path_to_private_key)->value_name(ProgramOptions::PRIVATE_KEY_VALUE_NAME.data()),
       ProgramOptions::PRIVATE_KEY_DESC.data());

  client_options_description.add_options()
      (ProgramOptions::ADDRESSES.data(),
       po::value(&args_.addresses)->multitoken()->value_name(ProgramOptions::ADDRESSES_VALUE_NAME.data()),
       ProgramOptions::ADDRESSES_DESC.data())
      (ProgramOptions::PACKAGES.data(),
       po::value(&args_.path_to_packages)->multitoken()->value_name(ProgramOptions::PACKAGES_VALUE_NAME.data()),
       ProgramOptions::PACKAGES_DESC.data());

  po::variables_map vm;
  po::parsed_options parsed_options = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
  po::store(parsed_options, vm);
  po::notify(vm);

  if (vm.contains(ProgramOptions::HELP.data())) {
    desc.add(server_options_description).add(client_options_description);
    std::cout << desc;

    return std::make_unique<config::NullConfig>();
  }

  if (!vm.contains(ProgramOptions::MODE.data())) {
    throw std::runtime_error(ExceptionMessage::OPTION_MODE_IS_MISSED.data());
  }

  std::unique_ptr<config::Config> config;

  if (args_.mode == ProgramOptions::MODE_SERVER.data()) {
    desc.add(server_options_description);
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!vm.contains(ProgramOptions::PORT.data()) ||
        !vm.contains(ProgramOptions::PUBLIC_KEY.data()) ||
        !vm.contains(ProgramOptions::PRIVATE_KEY.data())) {
      throw std::runtime_error(ExceptionMessage::WRONG_SERVER_CONFIG.data());
    }

    if (args_.port < 0 || args_.port > 65535) {
      throw std::runtime_error(constants::ExceptionMessage::NON_EXISTENT_PORT.data());
    }

    config = std::make_unique<config::ServerConfig>(args_.port, args_.path_to_public_key, args_.path_to_private_key);
  } else if (args_.mode == ProgramOptions::MODE_CLIENT.data()) {
    desc.add(client_options_description);
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (!vm.contains(ProgramOptions::ADDRESSES.data()) || !vm.contains(ProgramOptions::PACKAGES.data())) {
      throw std::runtime_error(ExceptionMessage::WRONG_CLIENT_CONFIG.data());
    }

    config = std::make_unique<config::ClientConfig>(ParseAddresses(), ParsePaths());
  } else {
    desc.add(server_options_description).add(client_options_description);
    std::cout << desc << std::endl;

    throw std::runtime_error(ExceptionMessage::UNKNOWN_MODE.data());
  }

  return config;
}

config::ClientConfig::Address CommandLineParser::ConvertStrToAddress(const std::string &str) {
  std::string delimiter = constants::ProgramOptions::ADDRESSES_DELIMITER.data();
  auto delimiter_pos = str.find(delimiter);

  if (delimiter_pos == std::string::npos || delimiter_pos == 0) {
    throw std::runtime_error(constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
  }

  std::string ip = str.substr(0, delimiter_pos);

  int port = -1;

  try {
    port = std::stoi(str.substr(delimiter_pos + delimiter.size(), str.size() - delimiter_pos));
  } catch (...) {
    throw std::runtime_error(constants::ExceptionMessage::WRONG_ADDRESS_FORMAT.data());
  }

  boost::system::error_code ec;
  boost::asio::ip::make_address(ip, ec);

  if (ec) {
    throw std::runtime_error(constants::ExceptionMessage::NON_EXISTENT_IP.data());
  }

  if (port < 0 || port > 65535) {
    throw std::runtime_error(constants::ExceptionMessage::NON_EXISTENT_PORT.data());
  }

  return {ip, port};
}

std::vector<fs::path> CommandLineParser::ParsePaths() {
  std::vector<fs::path> packages_path;

  for (auto &path_str : args_.path_to_packages) {
    packages_path.emplace_back(path_str);
  }

  if (packages_path.empty()) {
    throw std::runtime_error(constants::ExceptionMessage::NO_PACKAGES.data());
  }

  return std::move(packages_path);
}

std::vector<config::ClientConfig::Address> CommandLineParser::ParseAddresses() {
  std::vector<config::ClientConfig::Address> servers;

  for (const auto &address_str : args_.addresses) {
    servers.emplace_back(ConvertStrToAddress(address_str));
  }

  return std::move(servers);
}

}  // namespace util
