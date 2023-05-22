#include "arg_parser.hpp"

#include "vocabulary/size.hpp"
#include "vocabulary/time.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <concepts>
#include <string>
#include <string_view>
#include <vector>

namespace gfx::test
{
class CLI
{
  public:
    explicit CLI(std::convertible_to<std::string_view> auto&&... strings)
        : _argc{sizeof...(strings) + 1},
          _strings{"dummy", static_cast<const char*>(strings)...}
    {}

    [[nodiscard]] int argc() const
    {
      return _argc;
    }

    [[nodiscard]] const char* const* argv() const
    {
      return _strings.data();
    }

  private:
    int _argc;
    std::vector<const char*> _strings;
};
}

TEST_CASE("argparser_test", "[gfx][utils][arg_parser]")
{
  GIVEN("argc and argv with size")
  {
    const gfx::test::CLI cli{"--size", "32x128"};
    THEN("get size struct from arg parser")
    {
      auto result = gfx::utils::ArgParser(cli.argc(), cli.argv());
      REQUIRE(result.getSize() == gfx::Size{32, 128});
    }
  }

  GIVEN("argc and argv with duration")
  {
    const gfx::test::CLI cli{"--duration", "15"};
    THEN("get size struct from arg parser")
    {
      auto result = gfx::utils::ArgParser(cli.argc(), cli.argv());
      REQUIRE(result.getDuration() == gfx::time::sec(15));
    }
  }

  GIVEN("argc and argv with verbose")
  {
    const gfx::test::CLI cli{"--verbose"};
    THEN("get size struct from arg parser")
    {
      auto result = gfx::utils::ArgParser(cli.argc(), cli.argv());
      REQUIRE(result.getVerbose() == true);
    }
  }

  GIVEN("argc and argv with frame rate")
  {
    const gfx::test::CLI cli{"--frame-rate", "30"};
    THEN("get size struct from arg parser")
    {
      auto result = gfx::utils::ArgParser(cli.argc(), cli.argv());
      REQUIRE(result.getFrameRate() == gfx::time::fps(30));
    }
  }

  GIVEN("argc and argv with uri")
  {
    const gfx::test::CLI cli{"--input-uri", "unix:/tmp/test"};
    THEN("get uri string from arg parser")
    {
      auto result = gfx::utils::ArgParser(cli.argc(), cli.argv());
      REQUIRE(std::string{result.getInputUri().c_str()} == "unix:/tmp/test");
    }
  }
}
