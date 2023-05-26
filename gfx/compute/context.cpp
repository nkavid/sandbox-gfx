#include "context.hpp"

#include "detail/check_cuda_call.hpp"
#include "utils/logger.hpp"

#include <cuda.h>

#include <array>
#include <cstddef>

namespace gfx::compute
{
Context::Context()
{
  CUCHECK(cuInit(0));
  const int gpu{0};
  CUdevice cuDevice{0};
  CUCHECK(cuDeviceGet(&cuDevice, gpu));

  constexpr size_t deviceNameLength{80};
  std::array<char, deviceNameLength> deviceName{};
  CUCHECK(cuDeviceGetName(deviceName.data(), deviceName.size(), gpu));
  utils::logger::info("GPU in use: ", deviceName.data());
  CUCHECK(cuCtxCreate(&_cuContext, CU_CTX_SCHED_BLOCKING_SYNC, cuDevice));
}

Context::~Context()
{
  CUCHECK(cuCtxDestroy(_cuContext));
}

CUcontext Context::get() const
{
  return _cuContext;
}
} // namespace gfx::compute
