#include "quad.hpp"

#include <GL/glew.h>

#include <array>
#include <cstddef>

namespace gfx::graphics::detail
{
namespace attribute
{
enum attribute
{
  position,
  texCoord
};
}

constexpr size_t numCorners = 4;
constexpr size_t posDim     = 3;

static const std::array<float, numCorners* posDim> pos_coords = {
    {1.0F, 1.0F, 0.0F, 1.0F, -1.0F, 0.0F, -1.0F, -1.0F, 0.0F, -1.0F, 1.0F, 0.0F}
};

constexpr size_t texDim = 2;

static const std::array<float, numCorners* texDim> tex_coords = {
    {1.0F, 1.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F}
};

constexpr size_t numVertices = 6;

static const std::array<unsigned char, numVertices> indices = {
    {0, 1, 3, 1, 2, 3}
};
}

namespace gfx::graphics
{
Quad::Quad()
{
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  unsigned int pos_VBO{};
  glGenBuffers(1, &pos_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, pos_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(detail::pos_coords),
               detail::pos_coords.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(detail::attribute::position,
                        detail::posDim,
                        GL_FLOAT,
                        GL_FALSE,
                        detail::posDim * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(detail::attribute::position);

  unsigned int tex_VBO{};
  glGenBuffers(1, &tex_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, tex_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(detail::tex_coords),
               detail::tex_coords.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(detail::attribute::texCoord,
                        detail::texDim,
                        GL_FLOAT,
                        GL_FALSE,
                        detail::texDim * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(detail::attribute::texCoord);

  unsigned int EBO{};
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               detail::indices.size(),
               detail::indices.data(),
               GL_STATIC_DRAW);
}

void Quad::draw() const
{
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, detail::numVertices, GL_UNSIGNED_BYTE, nullptr);
}
}
