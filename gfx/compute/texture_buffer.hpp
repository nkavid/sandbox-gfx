#pragma once

#include <GL/glew.h>

#include <cudaGL.h>

namespace gfx
{
struct Size;
}

namespace gfx::compute
{
class TextureBuffer
{
  public:
    explicit TextureBuffer(const gfx::Size& size);
    ~TextureBuffer();

    TextureBuffer operator=(const TextureBuffer&) = delete;
    TextureBuffer(const TextureBuffer&)           = delete;
    TextureBuffer operator=(TextureBuffer&&)      = delete;
    TextureBuffer(TextureBuffer&&)                = delete;

    [[nodiscard]] CUarray lockCuArray();
    void releaseCuArray();
    [[nodiscard]] GLuint getTexture() const;

  private:
    CUgraphicsResource _cuResource{};
    GLuint _texture{};
};
}
