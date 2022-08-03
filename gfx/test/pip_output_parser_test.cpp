#include "pip_output_parser.hpp"

#include <catch2/catch_test_macros.hpp>

#include <sstream>

TEST_CASE("installed package does not match", "[gfx][utils][pip_output_parser]")
{
  std::stringstream inputStream;
  inputStream << "Requirement already satisfied: "
              << "bleee>=1.2.3"
              << " from -r requirements.txt "
              << "(4.5.6)";

  std::stringstream outputStream;
  REQUIRE(gfx::utils::parse_pip_output(inputStream, outputStream) == true);

  REQUIRE(outputStream.str()
          == "bleee required 1.2.3 does not match installed 4.5.6\n");
}

TEST_CASE("would install new or updated package", "[gfx][utils][pip_output_parser]")
{
  std::stringstream inputStream;
  inputStream << "Would install "
              << "glake8-someplugin-1.23.0";

  std::stringstream outputStream;
  REQUIRE(gfx::utils::parse_pip_output(inputStream, outputStream) == true);

  REQUIRE(outputStream.str() == "would install: glake8-someplugin-1.23.0\n");
}
