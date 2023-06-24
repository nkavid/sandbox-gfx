#include "color.hpp"

#include "color_stream_operator.hpp"

#include <catch2/catch_test_macros.hpp>

#include <bit>
#include <cstdint>

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
      STATIC_REQUIRE(color.alpha() == 1.0F);
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
      STATIC_REQUIRE(color.alpha() == 1.0F);
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
      STATIC_REQUIRE(color.alpha() == 1.0F);
    }
  }
}

SCENARIO("Overloaded 'operator+()' implements blending", "[gfx][color]")
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
        STATIC_REQUIRE(resultColor == solidColor);
      }
    }
  }
}

SCENARIO("Equivalence operator for rgb channels", "[gfx][color]")
{
  GIVEN("An instances color by rotating hexcode")
  {
    constexpr uint32_t hexCode{0x12ABABAB};

    constexpr gfx::Color aColor{std::rotr(hexCode, 8 * 0)};
    constexpr gfx::Color bColor{std::rotr(hexCode, 8 * 1)};
    constexpr gfx::Color cColor{std::rotr(hexCode, 8 * 2)};
    constexpr gfx::Color dColor{std::rotr(hexCode, 8 * 3)};

    WHEN("Rotating another color instance hexcode")
    {
      constexpr gfx::Color xColor{std::rotr(hexCode, 8 * 0)};
      constexpr gfx::Color yColor{std::rotr(hexCode, 8 * 1)};
      constexpr gfx::Color zColor{std::rotr(hexCode, 8 * 2)};
      constexpr gfx::Color wColor{std::rotr(hexCode, 8 * 3)};

      THEN("check each channel for equivalence")
      {
        STATIC_REQUIRE(aColor == xColor);
        STATIC_REQUIRE(aColor != yColor);
        STATIC_REQUIRE(aColor != zColor);
        STATIC_REQUIRE(aColor != wColor);

        STATIC_REQUIRE(bColor != xColor);
        STATIC_REQUIRE(bColor == yColor);
        STATIC_REQUIRE(bColor != zColor);
        STATIC_REQUIRE(bColor != wColor);

        STATIC_REQUIRE(cColor != xColor);
        STATIC_REQUIRE(cColor != yColor);
        STATIC_REQUIRE(cColor == zColor);
        STATIC_REQUIRE(cColor != wColor);

        STATIC_REQUIRE(dColor != xColor);
        STATIC_REQUIRE(dColor != yColor);
        STATIC_REQUIRE(dColor != zColor);
        STATIC_REQUIRE(dColor == wColor);
      }
    }
  }
}

SCENARIO("Equivalence operator for alpha channels", "[gfx][color]")
{
  GIVEN("two colors with different alpha channels")
  {
    constexpr gfx::Color aColor{1.0F, 1.0F, 1.0F, 0.75F};
    constexpr gfx::Color bColor{1.0F, 1.0F, 1.0F, 0.25F};

    THEN("colors are not equal")
    {
      STATIC_REQUIRE(aColor != bColor);
    }
  }
}

SCENARIO("Color channel equivalence", "[gfx][color]")
{
  GIVEN("three color channels with different values")
  {
    constexpr uint32_t channelValue{0x33};
    constexpr gfx::Channel channel{channelValue};

    THEN("colors are not equal")
    {
      STATIC_REQUIRE(channel != static_cast<float>(channelValue + 2) / 255.0F);
      STATIC_REQUIRE(channel != static_cast<float>(channelValue - 2) / 255.0F);
    }
  }
}

// NOLINTEND(readability-magic-numbers)
