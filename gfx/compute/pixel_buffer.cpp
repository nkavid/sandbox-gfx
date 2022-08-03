#include "pixel_buffer.hpp"

#include "detail/check_cuda_call.hpp"
#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <cudaGL.h>

namespace gfx::compute
{
PixelBuffer::PixelBuffer(const gfx::Size& size)
    : PixelBuffer::PixelBuffer(size.width, size.height)
{
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
PixelBuffer::PixelBuffer(size_t width, size_t height)
    : _width{width},
      _height{height}
{
  glGenBuffers(1, &_pbo);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);

  constexpr unsigned int channels{4};

  glBufferData(GL_PIXEL_UNPACK_BUFFER,
               static_cast<GLsizeiptr>(_width * _height * channels),
               nullptr,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  CUCHECK(cuGraphicsGLRegisterBuffer(&_cuResource,
                                     _pbo,
                                     CU_GRAPHICS_REGISTER_FLAGS_NONE));
}

PixelBuffer::~PixelBuffer()
{
  CUCHECK(cuGraphicsUnregisterResource(_cuResource));
  glDeleteBuffers(1, &_pbo);
}

void PixelBuffer::blitToTexture(unsigned int destination) const
{
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pbo);
  glBindTexture(GL_TEXTURE_2D, destination);
  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  0,
                  0,
                  static_cast<int>(_width),
                  static_cast<int>(_height),
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void PixelBuffer::lockDevicePtr(CUdeviceptr* devicePtr)
{
  CUCHECK(cuGraphicsMapResources(1, &_cuResource, nullptr));
  size_t size = 0;
  CUCHECK(cuGraphicsResourceGetMappedPointer(devicePtr, &size, _cuResource));
}

CUdeviceptr PixelBuffer::lockDevicePtr()
{
  CUCHECK(cuGraphicsMapResources(1, &_cuResource, nullptr));

  CUdeviceptr devicePtr{};
  size_t size = 0;
  CUCHECK(cuGraphicsResourceGetMappedPointer(&devicePtr, &size, _cuResource));

  return devicePtr;
}

void PixelBuffer::releaseDevicePtr()
{
  CUCHECK(cuGraphicsUnmapResources(1, &_cuResource, nullptr));
}
}
