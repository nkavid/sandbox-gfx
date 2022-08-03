#pragma once

#include <cuda.h>

#include <string_view>

namespace gfx
{
class Size;
}

namespace gfx::compute::utils
{
void dump_deviceptr(std::string_view filename,
                    CUdeviceptr deviceptr,
                    const gfx::Size& size);
}
