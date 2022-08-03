#pragma once

#include <cuda.h>

namespace gfx::compute
{
void callDrawCircleTexture(CUarray array, size_t width, size_t height);
}
