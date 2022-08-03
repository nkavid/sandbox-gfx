#include "utils/arg_parser.hpp"
#include "utils/muxer.hpp"
#include "vocabulary/time.hpp"

#include <cstdio>
#include <cstdlib>
#include <exception>

int main(int argc, const char* const* argv)
{
  try
  {
    const gfx::utils::ArgParser argParser{argc, argv};
    gfx::utils::video::Muxer{argParser.getOutputUri(),
                             argParser.getSize(),
                             gfx::time::as_ms(argParser.getDuration()),
                             argParser.getFrameRate()};
  }
  catch (const std::exception& e)
  {
    std::puts(e.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
