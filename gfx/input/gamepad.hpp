#pragma once

#include <GLFW/glfw3.h>

#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>

namespace gfx::input
{
namespace button
{
enum Name
{
  A,
  B,
  Y,
  X,
  LEFT_BUMPER,
  RIGHT_BUMPER,
  SELECT,
  START,
  UNUSED,
  LEFT_STICK,
  RIGHT_STICK,
  UP,
  RIGHT,
  DOWN,
  LEFT
};
}

struct Stick
{
    std::string _name{};

    explicit Stick(const char* name)
        : _name{name}
    {}

    struct Data
    {
        float x;
        float y;
    } _data{};

    bool update(const float* data)
    {
      constexpr unsigned xyDims{2};
      // NOLINTNEXTLINE(bugprone-suspicious-memory-comparison)
      if (std::memcmp(&_data, data, sizeof(float) * xyDims) != 0)
      {
        std::memcpy(&_data, data, sizeof(float) * xyDims);
        return true;
      }

      return false;
    }

    void print() const
    {
      std::cout << _name << "_X: " << _data.x << '\n';
      std::cout << _name << "_Y: " << _data.y << '\n';
    }
};

struct Trigger
{
    bool pressed{};
    int cache{};

    bool update(const float* data)
    {
      auto intdata = static_cast<int>(std::round(*data));
      if (cache != intdata)
      {
        cache   = intdata;
        pressed = (intdata == 1);
        return true;
      }

      return false;
    }
};

struct DirectionalPad
{
    bool up{};
    bool down{};
    bool left{};
    bool right{};
    bool updated{};
};

class Gamepad
{
  public:
    Gamepad();

    void update();
    void printAxes();

  private:
    int _glfwJoystickId{GLFW_JOYSTICK_1};
    Stick _leftStick{"LEFT_STICK"};
    Stick _rightStick{"RIGHT_STICK"};
    Trigger _leftTrigger{};
    Trigger _rightTrigger{};
    DirectionalPad _directionalPad{};

    void _invalidate();

    constexpr static unsigned int directions{4};
    using DirectionalPadData = std::array<unsigned char, directions>;
    void _updateDirectionalPad(const DirectionalPadData& newButtons);
};
}
