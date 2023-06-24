#include "color.hpp"

#include <ostream>

[[maybe_unused]] static std::ostream& operator<<(std::ostream& outStream,
                                                 const gfx::Color& color)
{
  outStream << "[" << color.red() << ", " << color.green() << ", " << color.blue()
            << ", " << color.alpha() << "]";
  return outStream;
}
