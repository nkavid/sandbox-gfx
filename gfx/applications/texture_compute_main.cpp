#include "compute/circle_texture.cuh"
#include "compute/context.hpp"
#include "compute/texture_buffer.hpp"
#include "graphics/quad.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "graphics/window.hpp"
#include "shaders/texture.hpp"
#include "utils/arg_parser.hpp"

#include <GL/glew.h>

#include <cstdlib>

int main(int argc, char** argv)
{
  using namespace gfx;
  utils::ArgParser const argParser{argc, argv};
  graphics::Window window{
      __FILE__,
      gfx::Size{1080, 720}
  };
  compute::Context const context{};

  gfx::Size const surfaceSize{512, 512};
  compute::TextureBuffer textureBuffer{surfaceSize};

  graphics::Quad const quad{};
  graphics::Shader const shader(graphics::shaders::Texture::vertex,
                                graphics::shaders::Texture::fragment);

  shader.use();
  const float scale = 0.8F;
  shader.setMatrixUniform(scale);

  compute::callDrawCircleTexture(textureBuffer.lockCuArray(),
                                 surfaceSize.width,
                                 surfaceSize.height);

  textureBuffer.releaseCuArray();

  glBindTexture(GL_TEXTURE_2D, textureBuffer.getTexture());

  do
  {
    quad.draw();
    window.swap();
  } while (window.isOpen());

  return EXIT_SUCCESS;
}
