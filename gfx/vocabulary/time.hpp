#pragma once

#include <chrono>

namespace gfx::time
{
using sec = std::chrono::duration<float, std::ratio<1>>;

using ms = std::chrono::milliseconds;

[[maybe_unused]] static ms as_ms(sec seconds)
{
  return std::chrono::duration_cast<ms>(seconds);
};

using fps = uint32_t;
} // namespace gfx::time
