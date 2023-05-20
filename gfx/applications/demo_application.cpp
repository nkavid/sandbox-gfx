#include "demo_application.hpp"

#include "compute/circle.cuh"
#include "compute/context.hpp"
#include "compute/pixel_buffer.hpp"
#include "compute/utils/compute_dump.hpp"
#include "graphics/quad.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "graphics/utils/graphics_dump.hpp"
#include "graphics/window.hpp"
#include "shaders/texture.hpp"
#include "utils/timestamp.hpp"
#include "vocabulary/color.hpp"
#include "vocabulary/size.hpp"

#include <cudaGL.h>

#include <string>

namespace gfx
{
// NOLINTNEXTLINE(readability-function-size)
void DemoApplication::run()
{
  constexpr gfx::Size windowSize{1200, 800};
  graphics::Window window{"demo application", windowSize};

  const graphics::Quad quad{};
  const graphics::Shader shader(graphics::shaders::Texture::vertex,
                                graphics::shaders::Texture::fragment);

  shader.use();
  const float scale{0.8F};
  shader.setMatrixUniform(scale);

  constexpr gfx::Color red{0xFF0000U};

  shader.setColorUniform(red);

  const compute::Context context{};

  constexpr gfx::Size drawSize{512, 512};
  compute::PixelBuffer pixelBuffer{drawSize};
  const graphics::Texture texture{drawSize};

  CUdeviceptr devicePtr{};
  pixelBuffer.lockDevicePtr(&devicePtr);
  compute::callDrawCircle(devicePtr, drawSize.width, drawSize.height);
  pixelBuffer.releaseDevicePtr();

  pixelBuffer.blitToTexture(texture.get());

  auto timestamp = utils::getTimestamp();

  utils::dump_texture(timestamp + "_dump_texture.jpg",
                      texture.get(),
                      drawSize.width,
                      drawSize.height);

  compute::utils::dump_deviceptr(timestamp + "_dump_deviceptr.jpg",
                                 devicePtr,
                                 drawSize);

  texture.bind();

  while (window.isOpen())
  {
    quad.draw();
    window.swap();
  }
}
}
