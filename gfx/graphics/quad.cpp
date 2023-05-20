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
}

struct detail
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
               sizeof(detail::pos_coords),
               detail::pos_coords.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(attribute::position,
                        detail::posDim,
                        GL_FLOAT,
                        GL_FALSE,
                        detail::posDim * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(attribute::position);

  unsigned int tex_VBO{};
  glGenBuffers(1, &tex_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, tex_VBO);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(detail::tex_coords),
               detail::tex_coords.data(),
               GL_STATIC_DRAW);

  glVertexAttribPointer(attribute::texCoord,
                        detail::texDim,
                        GL_FLOAT,
                        GL_FALSE,
                        detail::texDim * sizeof(float),
                        nullptr);
  glEnableVertexAttribArray(attribute::texCoord);

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
