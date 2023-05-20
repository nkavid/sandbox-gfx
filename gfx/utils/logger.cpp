#include "logger.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace gfx::utils::logger
{
namespace
{
std::string get_name()
{
  std::string exec_name{};
  std::ifstream("/proc/self/comm") >> exec_name;
  if (exec_name.size() > 14) // NOLINT
  {
    exec_name[12] = '.';  // NOLINT
    exec_name[13] = '.';  // NOLINT
    exec_name[14] = '.';  // NOLINT
    exec_name[15] = '\0'; // NOLINT
  }
  return exec_name;
}

constexpr std::string_view gfx = "gfx::";

struct ansi
{
    constexpr static std::string_view red    = "\x1B[31m";
    constexpr static std::string_view yellow = "\x1B[33m";
    constexpr static std::string_view cyan   = "\x1B[36m";
    constexpr static std::string_view white  = "\x1B[39m";
    constexpr static std::string_view bold   = "\x1B[1m";
    constexpr static std::string_view reset  = "\x1B[0m";
};

constexpr auto apply = [](std::string_view color) {
  return [color](std::string_view string) {
    std::stringstream sstream;
    sstream << color << string << ansi::white;
    return sstream.str();
  };
};

std::string getPreamble(std::string_view color, std::string_view tag)
{
  std::stringstream preamble;
  preamble << ansi::bold;
  preamble << gfx;
  preamble << get_name();
  preamble << " [";
  preamble << apply(color)(tag);
  preamble << "] ";
  preamble << ansi::reset;
  return preamble.str();
}
}

void fatal(std::string_view msg)
{
  const std::string preamble = getPreamble(ansi::red, "FATAL");
  std::cerr << preamble << msg << '\n';
  std::abort();
}

void fatal(std::string_view msg, std::string_view info)
{
  const std::string preamble = getPreamble(ansi::red, "FATAL");
  std::cerr << preamble << msg << info << '\n';
  std::abort();
}

void error(std::string_view msg)
{
  const std::string preamble = getPreamble(ansi::red, "ERROR");
  std::cerr << preamble << msg << '\n';
}

void warning(std::string_view msg)
{
  const std::string preamble = getPreamble(ansi::yellow, "WARNING");
  std::cerr << preamble << msg << '\n';
}

void info(std::string_view msg)
{
  const std::string preamble = getPreamble(ansi::cyan, "INFO");
  std::cout << preamble << msg << '\n';
}

void info(std::string_view msg, std::string_view info)
{
  const std::string preamble = getPreamble(ansi::cyan, "INFO");
  std::cout << preamble << msg << info << '\n';
}
}
