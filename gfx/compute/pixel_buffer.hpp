#pragma once

#include <GL/glew.h>

#include <cudaGL.h>

#include <cstddef>

namespace gfx
{
class Size;
}

namespace gfx::compute
{
class PixelBuffer
{
  public:
    explicit PixelBuffer(const gfx::Size& size);
    PixelBuffer(size_t width, size_t height);
    ~PixelBuffer();

    PixelBuffer(const PixelBuffer&)           = delete;
    PixelBuffer operator=(const PixelBuffer&) = delete;
    PixelBuffer(PixelBuffer&&)                = delete;
    PixelBuffer operator=(PixelBuffer&&)      = delete;

    void blitToTexture(unsigned int destination) const;

    void lockDevicePtr(CUdeviceptr* devicePtr);
    [[nodiscard]] CUdeviceptr lockDevicePtr();
    void releaseDevicePtr();

  private:
    size_t _width{};
    size_t _height{};
    CUgraphicsResource _cuResource{};
    unsigned int _pbo{0};
};
}
