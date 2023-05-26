#pragma once

#include <string_view>

namespace gfx::utils::logger
{
[[noreturn]] void fatal(std::string_view msg);
[[noreturn]] void fatal(std::string_view msg, std::string_view info);

void error(std::string_view msg);

void warning(std::string_view msg);

void info(std::string_view msg);
void info(std::string_view msg, std::string_view info);
} // namespace gfx::utils::logger
