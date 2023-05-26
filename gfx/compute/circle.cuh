#pragma once

#include <cuda.h>

namespace gfx::compute
{
void callDrawCircle(CUdeviceptr array, size_t width, size_t height);
} // namespace gfx::compute
