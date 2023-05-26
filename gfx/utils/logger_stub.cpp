#include "logger.hpp"

#include <stdexcept>
#include <string_view>

namespace gfx::utils::logger
{
void fatal(std::string_view /*msg*/)
{
  throw std::runtime_error("called [[noreturn]] stub");
}

void fatal(std::string_view /*msg*/, std::string_view /*info*/)
{
  throw std::runtime_error("called [[noreturn]] stub");
}

void error(std::string_view /*msg*/) {}

void warning(std::string_view /*msg*/) {}

void info(std::string_view /*msg*/) {}

void info(std::string_view /*msg*/, std::string_view /*info*/) {}
}
