#include "texture_buffer.hpp"

#include "detail/check_cuda_call.hpp"
#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <cudaGL.h>

namespace gfx::compute
{
TextureBuffer::TextureBuffer(const gfx::Size& size)
{
  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA32F,
               static_cast<GLsizei>(size.width),
               static_cast<GLsizei>(size.height),
               0,
               GL_RGBA,
               GL_FLOAT,
               nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);

  CUCHECK(cuGraphicsGLRegisterImage(&_cuResource,
                                    _texture,
                                    GL_TEXTURE_2D,
                                    CU_GRAPHICS_REGISTER_FLAGS_WRITE_DISCARD));
}

TextureBuffer::~TextureBuffer()
{
  CUCHECK(cuGraphicsUnregisterResource(_cuResource));
  glDeleteTextures(1, &_texture);
}

CUarray TextureBuffer::lockCuArray()
{
  CUCHECK(cuGraphicsMapResources(1, &_cuResource, nullptr));

  CUarray cuArray{};
  CUCHECK(cuGraphicsSubResourceGetMappedArray(&cuArray, _cuResource, 0, 0));

  return cuArray;
}

void TextureBuffer::releaseCuArray()
{
  CUCHECK(cuGraphicsUnmapResources(1, &_cuResource, nullptr));
}

GLuint TextureBuffer::getTexture() const
{
  return _texture;
}
} // namespace gfx::compute
