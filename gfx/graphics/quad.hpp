#pragma once

namespace gfx::graphics
{
class Quad
{
  public:
    Quad();
    void draw() const;

  private:
    unsigned int _vao{0};
};
} // namespace gfx::graphics
