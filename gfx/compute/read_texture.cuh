#pragma once

#include "vocabulary/size.hpp"

#include <cuda.h>

namespace gfx::compute
{
void readFromTexture(CUarray array,
                     const Size& arraySize,
                     CUdeviceptr devicePtr,
                     const Size& devicePtrSize);
} // namespace gfx::compute
