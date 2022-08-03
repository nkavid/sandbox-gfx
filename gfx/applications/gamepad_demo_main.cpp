#include "graphics/window.hpp"
#include "input/gamepad.hpp"
#include "vocabulary/size.hpp"

#include <cstdlib>

int main()
{
  using namespace gfx;
  constexpr gfx::Size squareWindowSize{512, 512};
  graphics::Window window{"gamepad", squareWindowSize};

  input::Gamepad gamepad{};

  while (window.isOpen())
  {
    gamepad.update();
    gamepad.printAxes();
    window.swap();
  }
  return EXIT_SUCCESS;
}
