#include "compute/context.hpp"
#include "compute/detail/check_cuda_call.hpp"
#include "compute/read_texture.cuh"
#include "compute/render_buffer.hpp"
#include "compute/utils/compute_dump.hpp"
#include "graphics/window.hpp"
#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <cudaGL.h>

#include <cstdint>
#include <cstdlib>
#include <tuple>

namespace
{
void draw_some_stuff()
{
  // NOLINTBEGIN(readability-magic-numbers)
  glClearColor(1.0F, 1.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_SCISSOR_TEST);

  glClearColor(1.0F, 0.0F, 1.0F, 1.0F);
  glScissor(100, 200, 20, 100);
  glClear(GL_COLOR_BUFFER_BIT);
  glScissor(300, 200, 20, 100);
  glClear(GL_COLOR_BUFFER_BIT);

  glClearColor(0.0F, 1.0F, 1.0F, 1.0F);
  glScissor(100, 100, 300, 20);
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_SCISSOR_TEST);
  // NOLINTEND(readability-magic-numbers)
}

std::tuple<CUdeviceptr, size_t> alloc_pitch_deviceptr(const gfx::Size& devicePtrSize,
                                                      size_t numChannels)
{
  CUdeviceptr devicePtr{};
  size_t devicePtrPitch{0UL};

  CUCHECK(cuMemAllocPitch(&devicePtr,
                          &devicePtrPitch,
                          devicePtrSize.width * numChannels,
                          devicePtrSize.height,
                          static_cast<uint32_t>(numChannels)));

  return std::make_tuple(devicePtr, devicePtrPitch);
}

} // namespace

int main()
{
  constexpr gfx::Size windowSize{1200UL, 800UL};
  const gfx::graphics::Window window{__FILE__,
                                     windowSize,
                                     gfx::graphics::Window::Mode::Headless};

  const gfx::compute::Context context{};

  constexpr gfx::Size renderBufferSize{512UL, 512UL};
  gfx::compute::RenderBuffer renderBuffer{renderBufferSize};

  renderBuffer.bindFrameBuffer();
  draw_some_stuff();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  constexpr gfx::Size devicePtrSize{512UL, 512UL};
  constexpr size_t numChannels{4UL};

  auto [devicePtr, devicePtrPitch] = alloc_pitch_deviceptr(devicePtrSize, numChannels);

  auto* renderBufferArray = renderBuffer.lockCuArray();

  CUDA_MEMCPY2D memcpyStruct{};
  memcpyStruct.Height        = devicePtrSize.width;
  memcpyStruct.WidthInBytes  = devicePtrSize.width * numChannels;
  memcpyStruct.srcMemoryType = CU_MEMORYTYPE_ARRAY;
  memcpyStruct.srcArray      = renderBufferArray;
  memcpyStruct.dstMemoryType = CU_MEMORYTYPE_DEVICE;
  memcpyStruct.dstDevice     = devicePtr;

  CUCHECK(cuMemcpy2D(&memcpyStruct));

  gfx::compute::utils::dump_deviceptr("headless_memcpy.jpg", devicePtr, devicePtrSize);

  gfx::compute::readFromTexture(renderBufferArray,
                                renderBufferSize,
                                devicePtr,
                                devicePtrSize);

  gfx::compute::utils::dump_deviceptr("headless_kernel.jpg", devicePtr, devicePtrSize);

  renderBuffer.releaseCuArray();

  CUCHECK(cuMemFree(devicePtr));
  return EXIT_SUCCESS;
}
