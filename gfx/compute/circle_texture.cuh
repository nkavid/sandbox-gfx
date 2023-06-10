#pragma once

#include <cuda.h>

#if defined __clang__
#error surf2Dwrite not available in Clang. \
"texObj" only for fetching/sampling, not write
#endif

namespace gfx::compute
{
void callDrawCircleTexture(CUarray array, size_t width, size_t height);
} // namespace gfx::compute
