#pragma once

#include "../../src/util/command_line_parser.h"

char **MakeArgv(const std::vector<std::string> &args);
void DeleteArgv(int argc, char **argv);
std::unique_ptr<util::Config> ParseFromVector(const std::vector<std::string> &arguments);
