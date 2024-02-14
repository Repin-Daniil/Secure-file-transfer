#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

void LoggerInit();

void LogFatal(std::string_view msg);
void LogInfo(std::string_view tag, std::string_view msg);
void LogError(std::string_view tag, std::string_view msg);
void LogTrace(std::string_view tag, std::string_view msg);
