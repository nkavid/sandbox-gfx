#include "arg_parser.hpp"

#include "vocabulary/size.hpp"
#include "vocabulary/time.hpp"
#include "vocabulary/uri.hpp"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/type_index/type_index_facade.hpp>

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TRYCATCH(ARG)                                                                  \
  try                                                                                  \
  {                                                                                    \
    (ARG);                                                                             \
  }                                                                                    \
  catch (const std::exception& e)                                                      \
  {                                                                                    \
    std::cerr << __FILE__ << ':' << __LINE__ << " " << e.what();                       \
  }

namespace gfx::utils
{
namespace
{
[[maybe_unused]] std::ostream& operator<<(std::ostream& outStream, const Size& size)
{
  outStream << static_cast<size_t>(size.width) << 'x'
            << static_cast<size_t>(size.height);
  return outStream;
}
} // namespace

void ArgParser::_checkForOutputPath()
{
  if (_vm.count("output-path") != 0)
  {
    TRYCATCH(_outputPath = _vm["output-path"].as<std::filesystem::path>())
  }
}

void ArgParser::_checkForInputPath()
{
  if (_vm.count("input-path") != 0)
  {
    TRYCATCH(_inputPath = _vm["input-path"].as<std::filesystem::path>())
  }
}

void ArgParser::_checkForSize()
{
  if (_vm.count("size") != 0)
  {
    auto sizeString = _vm["size"].as<std::string>();
    const std::string delimeter{'x'};
    auto split = sizeString.find(delimeter);
    if (split == std::string::npos)
    {
      throw std::invalid_argument{
          "gfx::missing 'x' delimeter in '--size' argument value (WxH)"};
    }
    auto width  = std::stoi(sizeString.substr(0, split));
    auto height = std::stoi(sizeString.substr(split + 1, std::string::npos));
    _size       = gfx::Size{width, height};
  }
}

void ArgParser::_checkForOutputUri()
{
  if (_vm.count("output-uri") != 0)
  {
    TRYCATCH(_outputUri = gfx::URI{_vm["output-uri"].as<std::string>()})
  }
}

void ArgParser::_checkForInputUri()
{
  if (_vm.count("input-uri") != 0)
  {
    TRYCATCH(_inputUri = gfx::URI{_vm["input-uri"].as<std::string>()})
  }
}

void ArgParser::_checkForDuration()
{
  if (_vm.count("duration") != 0)
  {
    TRYCATCH(_duration = gfx::time::sec{_vm["duration"].as<uint32_t>()})
  }
}

void ArgParser::_checkForFrameRate()
{
  if (_vm.count("frame-rate") != 0)
  {
    TRYCATCH(_frameRate = gfx::time::fps{_vm["frame-rate"].as<uint32_t>()})
  }
}

void ArgParser::_checkForVerbose()
{
  if (_vm.count("verbose") != 0)
  {
    TRYCATCH(_verbose = _vm["verbose"].as<bool>())
  }
}

ArgParser::ArgParser(int argc, const char* const* argv)
{
  namespace po = boost::program_options;

  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message");

  desc.add_options()("output-path",
                     po::value<std::filesystem::path>(),
                     "Path to output file");

  desc.add_options()("input-path",
                     po::value<std::filesystem::path>(),
                     "Path to input file");

  desc.add_options()("size", po::value<std::string>(), "Size as WxH");

  desc.add_options()("output-uri", po::value<std::string>(), "URI for output");

  desc.add_options()("input-uri", po::value<std::string>(), "URI for input");

  desc.add_options()("duration", po::value<uint32_t>(), "in seconds");

  desc.add_options()("frame-rate", po::value<uint32_t>(), "as fps/herz");

  desc.add_options()("verbose", po::bool_switch(), "enable more logging");

  po::store(po::parse_command_line(argc, argv, desc), _vm);
  po::notify(_vm);

  if (_vm.count("help") != 0)
  {
    std::cout << desc << "\n";
    std::abort();
  }

  _checkForOutputPath();
  _checkForInputPath();
  _checkForSize();
  _checkForOutputUri();
  _checkForInputUri();
  _checkForDuration();
  _checkForFrameRate();
  _checkForVerbose();
}

std::filesystem::path ArgParser::getOutputPath() const
{
  if (_outputPath.has_value())
  {
    return _outputPath.value();
  }
  throw std::invalid_argument{"gfx::missing '--output-path' argument value"};
}

std::filesystem::path ArgParser::getInputPath() const
{
  if (_inputPath.has_value())
  {
    return _inputPath.value();
  }
  throw std::invalid_argument{"gfx::missing '--input-path' argument value"};
}

gfx::Size ArgParser::getSize() const
{
  if (_size.has_value())
  {
    return _size.value();
  }
  throw std::invalid_argument{"gfx::missing '--size' argument value"};
}

gfx::URI ArgParser::getOutputUri() const
{
  if (_outputUri.has_value())
  {
    return _outputUri.value();
  }
  throw std::invalid_argument{"gfx::missing '--output-uri' argument value"};
}

gfx::URI ArgParser::getInputUri() const
{
  if (_inputUri.has_value())
  {
    return _inputUri.value();
  }
  throw std::invalid_argument{"gfx::missing '--input-uri' argument value"};
}

gfx::time::sec ArgParser::getDuration() const
{
  if (_duration.has_value())
  {
    return _duration.value();
  }
  throw std::invalid_argument{"gfx::missing '--duration' argument value"};
}

gfx::time::fps ArgParser::getFrameRate() const
{
  if (_frameRate.has_value())
  {
    return _frameRate.value();
  }
  throw std::invalid_argument{"gfx::missing '--frame-rate' argument value"};
}

bool ArgParser::getVerbose() const
{
  return _verbose;
}
} // namespace gfx::utils
