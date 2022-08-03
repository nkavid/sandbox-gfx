#include "texture.hpp"

#include <GL/glew.h>

namespace gfx::graphics
{
Texture::Texture(const gfx::Size& size, void* data)
    : Texture::Texture(size.width, size.height, data)
{
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Texture::Texture(size_t width, size_t height, void* data)
    : _size{width, height}
{
  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA8,
               _size.width,
               _size.height,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               data);
}

Texture::~Texture()
{
  glDeleteTextures(1, &_texture);
}

unsigned int Texture::get() const
{
  return _texture;
}

void Texture::bind() const
{
  glBindTexture(GL_TEXTURE_2D, _texture);
}
}
