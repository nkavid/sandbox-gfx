#pragma once

#include "utils/logger.hpp"

#include <array>
#include <cstring>

namespace gfx::shmem::detail
{
[[noreturn]] static void logAndAbort(const char* function)
{
  constexpr size_t length{32};
  std::array<char, length> errorString{};
  const auto* result = strerror_r(errno, errorString.data(), errorString.size());

  utils::logger::fatal(function, result);
}

static void checkForError(int returnValue, const char* function, int errorCode = -1)
{
  if (returnValue == errorCode)
  {
    logAndAbort(function);
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr)
static void const* const defaultCode = reinterpret_cast<void*>(-1);

static void checkForError(void* returnValue,
                          const char* function,
                          void const* const errorCode = defaultCode)
{
  if (returnValue == errorCode)
  {
    logAndAbort(function);
  }
}
}
