#include "read_texture.cuh"

#include <stdexcept>

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

  // TODO read from tex2d in device
}
} // namespace gfx::compute
