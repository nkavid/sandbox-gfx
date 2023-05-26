#include "quad.hpp"

#include <GL/glew.h>

#include <array>
#include <cstddef>

namespace gfx::graphics
{
namespace attribute
{
enum Attribute
{
  position = 0,
  texCoord = 1
};
} // namespace attribute

struct Detail
{
    constexpr static size_t numCorners = 4;
    constexpr static size_t posDim     = 3;

    constexpr static std::array<float, numCorners* posDim> pos_coords = {
        {1.0F, 1.0F, 0.0F, 1.0F, -1.0F, 0.0F, -1.0F, -1.0F, 0.0F, -1.0F, 1.0F, 0.0F}
    };

    constexpr static size_t texDim = 2;

    constexpr static std::array<float, numCorners* texDim> tex_coords = {
        {1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F}
    };

    constexpr static size_t numVertices = 6;

    constexpr static const std::array<unsigned char, numVertices> indices = {
        {0, 1, 3, 1, 2, 3}
    };
};

Quad::Quad()
{
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  unsigned int pos_VBO{};
  glGenBuffers(1, &pos_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, pos_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Detail::pos_coords),
               Detail::pos_coords.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(attribute::position,
                        Detail::posDim,
                        GL_FLOAT,
                        GL_FALSE,
                        Detail::posDim * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(attribute::position);

  unsigned int tex_VBO{};
  glGenBuffers(1, &tex_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, tex_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Detail::tex_coords),
               Detail::tex_coords.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(attribute::texCoord,
                        Detail::texDim,
                        GL_FLOAT,
                        GL_FALSE,
                        Detail::texDim * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(attribute::texCoord);

  unsigned int EBO{};
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               Detail::indices.size(),
               Detail::indices.data(),
               GL_STATIC_DRAW);
}

void Quad::draw() const
{
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, Detail::numVertices, GL_UNSIGNED_BYTE, nullptr);
}
} // namespace gfx::graphics
