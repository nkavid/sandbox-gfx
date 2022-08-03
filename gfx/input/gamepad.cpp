#include "gamepad.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOGBUTTON(ID, ARRAY)                                                           \
  if ((ARRAY)[(ID)] == GLFW_PRESS)                                                     \
  std::cout << #ID << '\n'

namespace gfx::input::glfw_util
{
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
static void joystick_callback(int jid, int event)
{
  if (event == GLFW_CONNECTED)
  {
    std::cout << jid << " Connected!\n";
  }
  else if (event == GLFW_DISCONNECTED)
  {
    std::cout << jid << " Disconnected!\n";
  }
}

static void printJoystickInfo(int _glfwJoystickId)
{
  if (glfwJoystickPresent(_glfwJoystickId) == GLFW_TRUE)
  {
    std::cout << _glfwJoystickId << " is present\n";
  }
  else
  {
    std::cout << _glfwJoystickId << " is NOT present\n";
  }

  std::cout << "GUID: " << glfwGetJoystickGUID(_glfwJoystickId) << '\n';

  if (glfwJoystickIsGamepad(_glfwJoystickId) == GLFW_TRUE)
  {
    std::cout << _glfwJoystickId << " is a gamepad\n";
    std::cout << "gamepad name: " << glfwGetGamepadName(_glfwJoystickId) << '\n';
  }
  else
  {
    std::cout << _glfwJoystickId << " is NOT gamepad\n";
    std::cout << "joystick name: " << glfwGetJoystickName(_glfwJoystickId) << '\n';
  }
}
}

namespace gfx::input
{
void Gamepad::_invalidate()
{
  _directionalPad.updated = false;
}

void Gamepad::_updateDirectionalPad(const DirectionalPadData& newButtons)
{
  constexpr auto ButtonEnumOffset = Button::UP;
  const bool glfwUp = newButtons[Button::UP - ButtonEnumOffset] == GLFW_PRESS;
  if (glfwUp != _directionalPad.up)
  {
    _directionalPad.updated = true;
    _directionalPad.up      = glfwUp;
    std::cout << "DPAD UP: " << _directionalPad.up << '\n';
  }

  const bool glfwDown = newButtons[Button::DOWN - ButtonEnumOffset] == GLFW_PRESS;
  if (glfwDown != _directionalPad.down)
  {
    _directionalPad.updated = true;
    _directionalPad.down    = glfwDown;
    std::cout << "DPAD DOWN: " << _directionalPad.down << '\n';
  }

  const bool glfwLeft = newButtons[Button::LEFT - ButtonEnumOffset] == GLFW_PRESS;
  if (glfwLeft != _directionalPad.left)
  {
    _directionalPad.updated = true;
    _directionalPad.left    = glfwLeft;
    std::cout << "DPAD LEFT: " << _directionalPad.left << '\n';
  }

  const bool glfwRight = newButtons[Button::RIGHT - ButtonEnumOffset] == GLFW_PRESS;
  if (glfwRight != _directionalPad.right)
  {
    _directionalPad.updated = true;
    _directionalPad.right   = glfwRight;
    std::cout << "DPAD RIGHT: " << _directionalPad.right << '\n';
  }
}

Gamepad::Gamepad()
{
  glfwSetJoystickCallback(glfw_util::joystick_callback);
  glfw_util::printJoystickInfo(_glfwJoystickId);
}

// NOLINTNEXTLINE(readability-function-size)
void Gamepad::update()
{
  _invalidate();

  int count{};
  const unsigned char* newButtons = glfwGetJoystickButtons(_glfwJoystickId, &count);
  if (newButtons == nullptr)
  {
    return;
  }

  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  LOGBUTTON(Button::UNUSED, newButtons);
  LOGBUTTON(Button::A, newButtons);
  LOGBUTTON(Button::B, newButtons);
  LOGBUTTON(Button::X, newButtons);
  LOGBUTTON(Button::Y, newButtons);
  LOGBUTTON(Button::START, newButtons);
  LOGBUTTON(Button::SELECT, newButtons);
  LOGBUTTON(Button::RIGHT_BUMPER, newButtons);
  LOGBUTTON(Button::LEFT_BUMPER, newButtons);
  LOGBUTTON(Button::RIGHT_STICK, newButtons);
  LOGBUTTON(Button::LEFT_STICK, newButtons);
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

  static DirectionalPadData dPad{};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  std::memcpy(dPad.data(), newButtons + Button::UP, dPad.size());
  _updateDirectionalPad(dPad);
}

void Gamepad::printAxes()
{
  int count{};
  const float* axes = glfwGetJoystickAxes(_glfwJoystickId, &count);
  if (axes == nullptr)
  {
    return;
  }

  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  if (_leftStick.update(axes + 0))
  {
    _leftStick.print();
  }

  // NOLINTNEXTLINE(readability-magic-numbers)
  if (_rightStick.update(axes + 3))
  {
    _rightStick.print();
  }

  // NOLINTNEXTLINE(readability-magic-numbers)
  if (_rightTrigger.update(axes + 5))
  {
    std::cout << "RIGHT_TRIGGER: " << _rightTrigger.pressed << '\n';
  }

  // NOLINTNEXTLINE(readability-magic-numbers)
  if (_leftTrigger.update(axes + 2))
  {
    std::cout << "RIGHT_TRIGGER: " << _leftTrigger.pressed << '\n';
  }
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}
}
