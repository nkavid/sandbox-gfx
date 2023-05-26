#include "circle.cuh"

#include <cuda.h>

__global__ static void drawCircle(uint8_t* array)
{
  int x        = (threadIdx.x + blockIdx.x * blockDim.x);
  int y        = (threadIdx.y + blockIdx.y * blockDim.y);
  size_t pitch = 512 * 4;
  int radius   = 256;

  int2 pos{x - radius, y - radius};

  if ((pos.x * pos.x + pos.y * pos.y) <= radius * radius)
  {
    array[x * 4 + y * pitch + 0] = 0xff;
    array[x * 4 + y * pitch + 2] = 0xff;
  }
  else
  {
    array[x * 4 + y * pitch + 1] = 0xff;
  }
}

namespace gfx::compute
{
void callDrawCircle(CUdeviceptr array, size_t width, size_t height)
{
  size_t threadDim = 32;
  dim3 block{static_cast<uint32_t>(width / threadDim),
             static_cast<uint32_t>(height / threadDim),
             1};
  dim3 thread{static_cast<uint32_t>(threadDim), static_cast<uint32_t>(threadDim), 1};
  drawCircle<<<block, thread>>>((uint8_t*)array);
}
} // namespace gfx::compute
