#include "size.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("size::test", "[gfx][detail][size]")
{
  constexpr gfx::Size size{256U, 512U};

  STATIC_REQUIRE(size.width == 256);
  STATIC_REQUIRE(size.height == 512);

  STATIC_REQUIRE(size.size() == 256UL * 512UL);
}
