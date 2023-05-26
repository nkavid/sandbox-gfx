#pragma once

#include "utils/logger.hpp"

#include <cuda.h>

#if not defined(__clang__)
#include <source_location>
#include <string>
#endif

#ifdef CUCHECK
#error gfx::figure out a better alternative
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CUCHECK(ARG) ::gfx::compute::detail::checkCudaCall(ARG)
#endif

namespace gfx::compute::detail
{
#if defined(__clang__)
static void checkCudaCall(CUresult /*result*/) {}
#else
static void
checkCudaCall(CUresult result,
              const std::source_location location = std::source_location::current())
{
  if (result != CUDA_SUCCESS)
  {
    std::string location_string{};
    gfx::utils::logger::error(location_string + location.file_name() + ':'
                              + std::to_string(location.line()));
    gfx::utils::logger::error(location.function_name());
    const char* szErrName = NULL;
    cuGetErrorName(result, &szErrName);
    gfx::utils::logger::error(szErrName);
    const char* szErrString = NULL;
    cuGetErrorString(result, &szErrString);
    gfx::utils::logger::error(szErrString);
  }
}
#endif
} // namespace gfx::compute::detail
