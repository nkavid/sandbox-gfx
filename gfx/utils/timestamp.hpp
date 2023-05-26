#pragma once

#include <fmt/chrono.h>

#include <chrono>
#include <string>

namespace gfx::utils
{
[[nodiscard]] static std::string getTimestamp()
{
  auto time = std::chrono::system_clock::now();
  return fmt::format("{:%Y%m%d_%H%M}", time);
}
} // namespace gfx::utils
