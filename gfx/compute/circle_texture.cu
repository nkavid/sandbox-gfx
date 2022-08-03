#include "circle_texture.cuh"

#include <cuda.h>

__global__ void drawCircleTexture(CUsurfObject surf)
{
  int x      = (threadIdx.x + blockIdx.x * blockDim.x);
  int y      = (threadIdx.y + blockIdx.y * blockDim.y);
  int radius = 256;

  float4 data = make_float4(0.0f, 0.0f, 0.0f, 0x00);

  int2 pos{x - radius, y - radius};

  if ((pos.x * pos.x + pos.y * pos.y) <= radius * radius)
  {
    data.x = 1.0F;
    data.z = 1.0F;
  }
  else
  {
    data.y = 1.0F;
  }
  surf2Dwrite(data, surf, x * sizeof(float4), y, cudaBoundaryModeZero);
}

namespace gfx::compute
{
void callDrawCircleTexture(CUarray array, size_t width, size_t height)
{
  CUsurfObject surfObj;

  CUDA_RESOURCE_DESC resDesc;
  memset(&resDesc, 0, sizeof(CUDA_RESOURCE_DESC));
  resDesc.resType          = CU_RESOURCE_TYPE_ARRAY;
  resDesc.res.array.hArray = array;

  cuSurfObjectCreate(&surfObj, &resDesc);

  size_t threadDim = 32;
  dim3 block{static_cast<uint32_t>(width / threadDim),
             static_cast<uint32_t>(height / threadDim),
             1};
  dim3 thread{static_cast<uint32_t>(threadDim), static_cast<uint32_t>(threadDim), 1};

  drawCircleTexture<<<block, thread>>>(surfObj);
}
}
