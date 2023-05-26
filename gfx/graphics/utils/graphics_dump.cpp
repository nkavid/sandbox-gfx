#include "graphics_dump.hpp"

#include "utils/logger.hpp"

#include <GL/glew.h>

#include <stb_image_write.h>

#include <cstddef>
#include <string_view>
#include <vector>

namespace gfx::utils
{
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void dump_texture(std::string_view filename,
                  unsigned int texture,
                  size_t width,
                  size_t height)
// NOLINTEND(bugprone-easily-swappable-parameters)
{
  glBindTexture(GL_TEXTURE_2D, texture);
  std::vector<unsigned char> buffer{};
  constexpr unsigned int channels{4};
  buffer.resize(width * height * channels);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_flip_vertically_on_write(static_cast<int>(true));

  if (filename.ends_with(".jpg"))
  {
    static const int quality{100};
    stbi_write_jpg(filename.data(),
                   static_cast<int>(width),
                   static_cast<int>(height),
                   channels,
                   buffer.data(),
                   quality);
  }
  else
  {
    logger::fatal("did not recognize extension for ", filename);
  }
}
} // namespace gfx::utils
