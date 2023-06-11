#pragma once

#include "vocabulary/size.hpp"

#include <GL/glew.h>

#include <cudaGL.h>

namespace gfx::compute
{
class RenderBuffer
{
  public:
    RenderBuffer(const gfx::Size& size);
    ~RenderBuffer();

    RenderBuffer operator=(const RenderBuffer&) = delete;
    RenderBuffer(const RenderBuffer&)           = delete;
    RenderBuffer operator=(RenderBuffer&&)      = delete;
    RenderBuffer(RenderBuffer&&)                = delete;

    [[nodiscard]] CUarray lockCuArray();
    void releaseCuArray();

    void bindFrameBuffer() const;

  private:
    CUgraphicsResource _cuResource{};
    GLuint _rbo{};
    GLuint _fbo{};
};
} // namespace gfx::compute
