#pragma once

namespace gfx
{
class Color;
}

namespace gfx::graphics
{
class Shader
{
  public:
    Shader(const char* vertex, const char* fragment);
    explicit Shader(const char* compute);

    void use() const;
    void setColorUniform(float red, float green, float blue) const;
    void setColorUniform(const gfx::Color& color) const;
    void setMatrixUniform(float scale) const;

  private:
    unsigned int _program{0};
    int _colorLoc{0};
    int _matrixLoc{0};
};
}
