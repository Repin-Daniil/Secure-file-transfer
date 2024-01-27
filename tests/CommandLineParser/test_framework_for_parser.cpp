#include "test_framework_for_parser.h"

char **MakeArgv(const std::vector<std::string> &args) {
  size_t argc = args.size();
  auto argv = new char *[argc];

  for (int i = 0; i < argc; ++i) {
    argv[i] = new char[args[i].size() + 1];
    std::strcpy(argv[i], args[i].c_str());
  }

  return argv;
}

void DeleteArgv(int argc, char **argv) {
  for (int i = 0; i < argc; ++i) {
    delete[] argv[i];
  }

  delete[] argv;
}

std::unique_ptr<util::Config> ParseFromVector(const std::vector<std::string> &arguments) {
  int argc = arguments.size();
  auto argv = MakeArgv(arguments);

  util::CommandLineParser parser;
  auto config = parser.ParseCommandLine(argc, argv);

  DeleteArgv(argc, argv);

  return config;
};