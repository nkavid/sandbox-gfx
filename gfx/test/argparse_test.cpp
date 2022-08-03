#include "parse.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("check argparse", "[gfx][utils][argparse]")
{
  // NOLINTBEGIN
  char a[]     = {'b', 'a', 'r', '\0'};
  char b[]     = {'-', 'v', '\0'};
  char* argv[] = {a, b};

  argparse_parse(2, argv);
  // NOLINTEND
}
