#pragma once

#include "vocabulary/size.hpp"

#include <cstddef>

namespace gfx::graphics
{
class Texture
{
  public:
    explicit Texture(const gfx::Size& size, void* data = nullptr);
    Texture(size_t width, size_t height, void* data = nullptr);
    ~Texture();

    Texture(const Texture&)           = delete;
    Texture operator=(const Texture&) = delete;
    Texture(Texture&&)                = delete;
    Texture operator=(Texture&&)      = delete;

    [[nodiscard]] unsigned int get() const;
    void bind() const;

  private:
    gfx::Size _size;
    unsigned int _texture{0};
};
}
