#include "render_buffer.hpp"

#include "detail/check_cuda_call.hpp"
#include "vocabulary/size.hpp"

#include <cudaGL.h>

#include <stdexcept>

namespace gfx::compute
{
RenderBuffer::RenderBuffer(const gfx::Size& size)
{
  glGenRenderbuffers(1, &_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, size.width, size.height);

  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_COLOR_ATTACHMENT0,
                            GL_RENDERBUFFER,
                            _rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    throw std::runtime_error(
        "RenderBuffer::RenderBuffer(): Framebuffer is not complete!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  CUCHECK(cuGraphicsGLRegisterImage(&_cuResource,
                                    _rbo,
                                    GL_RENDERBUFFER,
                                    CU_GRAPHICS_REGISTER_FLAGS_READ_ONLY));
}

RenderBuffer::~RenderBuffer()
{
  CUCHECK(cuGraphicsUnregisterResource(_cuResource));
  glDeleteRenderbuffers(1, &_rbo);
  glDeleteFramebuffers(1, &_fbo);
}

[[nodiscard]] CUarray RenderBuffer::lockCuArray()
{
  CUCHECK(cuGraphicsMapResources(1, &_cuResource, nullptr));

  CUarray cuArray{};
  CUCHECK(cuGraphicsSubResourceGetMappedArray(&cuArray, _cuResource, 0, 0));

  return cuArray;
}

void RenderBuffer::releaseCuArray()
{
  CUCHECK(cuGraphicsUnmapResources(1, &_cuResource, nullptr));
}

void RenderBuffer::bindFrameBuffer() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

} // namespace gfx::compute
