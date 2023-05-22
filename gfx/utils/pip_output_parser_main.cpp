#include "pip_output_parser.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
  try
  {
    while (gfx::utils::parse_pip_output(std::cin, std::cout))
    {}
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
