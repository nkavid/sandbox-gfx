#include "read_texture.cuh"

#include <cuda.h>

#include <stdexcept>

__global__ void readFromTextureKernel(CUtexObject texObj, uint8_t* array)
{
  int x        = (threadIdx.x + blockIdx.x * blockDim.x);
  int y        = (threadIdx.y + blockIdx.y * blockDim.y);
  size_t pitch = 512 * 4;

  auto val = tex2D<int4>(texObj, x, y);

  array[x * 4 + y * pitch + 0] = val.x;
  array[x * 4 + y * pitch + 1] = val.y;
  array[x * 4 + y * pitch + 2] = val.z;
}

namespace gfx::compute
{
void readFromTexture(CUarray array,
                     const Size& arraySize,
                     CUdeviceptr devicePtr,
                     const Size& devicePtrSize)
{
  if (arraySize != devicePtrSize)
  {
    throw std::runtime_error("gfx::compute::readFromTexture mismatch size");
  }

  CUtexObject texObj;

  CUDA_RESOURCE_DESC resDesc;
  memset(&resDesc, 0, sizeof(resDesc));
  resDesc.resType          = CU_RESOURCE_TYPE_ARRAY;
  resDesc.res.array.hArray = array;

  CUDA_TEXTURE_DESC texDesc;
  memset(&texDesc, 0, sizeof(texDesc));
  texDesc.addressMode[0]      = CU_TR_ADDRESS_MODE_CLAMP;
  texDesc.addressMode[1]      = CU_TR_ADDRESS_MODE_CLAMP;
  texDesc.addressMode[2]      = CU_TR_ADDRESS_MODE_CLAMP;
  texDesc.filterMode          = CU_TR_FILTER_MODE_POINT;
  texDesc.flags               = CU_TRSF_READ_AS_INTEGER;
  texDesc.maxAnisotropy       = 1;
  texDesc.mipmapFilterMode    = CU_TR_FILTER_MODE_LINEAR;
  texDesc.mipmapLevelBias     = 0;
  texDesc.minMipmapLevelClamp = 0;
  texDesc.maxMipmapLevelClamp = 0;

  cuTexObjectCreate(&texObj, &resDesc, &texDesc, nullptr);

  const size_t width{512UL};
  const size_t height{512UL};

  size_t threadDim = 32;
  dim3 block{static_cast<uint32_t>(width / threadDim),
             static_cast<uint32_t>(height / threadDim),
             1};
  dim3 thread{static_cast<uint32_t>(threadDim), static_cast<uint32_t>(threadDim), 1};

  readFromTextureKernel<<<block, thread>>>(texObj, (uint8_t*)devicePtr);
}
} // namespace gfx::compute
