#include "applications/dummy_texture.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "graphics/utils/graphics_dump.hpp"
#include "graphics/window.hpp"
#include "shaders/yuv.hpp"
#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <cstdlib>

int main()
{
  using namespace gfx;

  const graphics::Window window{"opengl_compute_shader",
                                dummy_texture::size,
                                graphics::Window::Mode::Headless};

  const graphics::Texture inputTexture{dummy_texture::size,
                                       dummy_texture::get().data()};
  const graphics::Texture outputTexture{dummy_texture::size};

  const graphics::Shader shader{graphics::shaders::Yuv::compute};

  shader.use();

  glBindImageTexture(0, inputTexture.get(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
  glBindImageTexture(1, outputTexture.get(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

  glDispatchCompute(dummy_texture::size.width, dummy_texture::size.height, 1);

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glActiveTexture(GL_TEXTURE1);
  utils::dump_texture("output.jpg",
                      outputTexture.get(),
                      dummy_texture::size.width,
                      dummy_texture::size.height);
  glActiveTexture(GL_TEXTURE0);
  utils::dump_texture("input.jpg",
                      inputTexture.get(),
                      dummy_texture::size.width,
                      dummy_texture::size.height);

  return EXIT_SUCCESS;
}
