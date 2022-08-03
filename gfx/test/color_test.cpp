#include "color.hpp"

#include "color_stream_operator.hpp"

#include <catch2/catch_test_macros.hpp>

// NOLINTBEGIN(readability-magic-numbers)
SCENARIO("Check different constructors", "[gfx][color]")
{
  GIVEN("float input '0.2F, 0.3F, 0.4F'")
  {
    constexpr gfx::Color color{0.2F, 0.3F, 0.4F};
    THEN("same return as constructor input")
    {
      STATIC_REQUIRE(color.red() == 0.2F);
      STATIC_REQUIRE(color.green() == 0.3F);
      STATIC_REQUIRE(color.blue() == 0.4F);
    }
  }
  GIVEN("unsigned integer input '127U, 20U, 100U'")
  {
    constexpr gfx::Color color{127U, 20U, 100U};
    THEN("return normalized by 255")
    {
      STATIC_REQUIRE(color.red() == 127.0F / 255.0F);
      STATIC_REQUIRE(color.green() == 20.0F / 255.0F);
      STATIC_REQUIRE(color.blue() == 100.0F / 255.0F);
    }
  }
  GIVEN("single integer '0x40F2A9'")
  {
    constexpr gfx::Color color{0x40F2A9};
    THEN("normalize hexcode by 255")
    {
      STATIC_REQUIRE(color.red() == static_cast<float>(0x40) / 255.0F);
      STATIC_REQUIRE(color.green() == static_cast<float>(0xF2) / 255.0F);
      STATIC_REQUIRE(color.blue() == static_cast<float>(0xA9) / 255.0F);
    }
  }
}

SCENARIO("Ove_rloaded 'operator+()' implements blending", "[gfx][color]")
{
  GIVEN("two instances of color; solid and transparent")
  {
    constexpr gfx::Color solidColor{1.0F, 0.0F, 0.0F};
    constexpr gfx::Color transparentColor{0.0F, 1.0F, 0.0F, 0.6F};
    WHEN("add transparent (src) onto solid (dst)")
    {
      constexpr auto resultColor = solidColor + transparentColor;
      THEN("resulting color is solid")
      {
        STATIC_REQUIRE(resultColor.red() == 0.4F);
        STATIC_REQUIRE(resultColor.green() == 0.6F);
        STATIC_REQUIRE(resultColor.blue() == 0.0F);
        STATIC_REQUIRE(resultColor.alpha() == 1.0F);
      }
    }
    WHEN("add solid (src) onto transparent (dst)")
    {
      constexpr auto resultColor = transparentColor + solidColor;
      THEN("resulting color is the same as solid")
      {
        REQUIRE(resultColor == solidColor);
      }
    }
  }
}

// NOLINTEND(readability-magic-numbers)
