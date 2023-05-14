#include "uri.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("uri file", "[gfx][uri]")
{
  const std::string input = "file:small_bunny_1080p_60fps.mp4";
  const gfx::URI uri{input};

  REQUIRE(uri.path().is_relative());
  REQUIRE(uri.schema() == gfx::URI::Schema::File);
}

TEST_CASE("uri unix", "[gfx][uri]")
{
  const std::string input = "unix:/tmp/filename";
  const gfx::URI uri{input};

  REQUIRE(uri.path().is_absolute());
  REQUIRE(uri.schema() == gfx::URI::Schema::Unix);
}
