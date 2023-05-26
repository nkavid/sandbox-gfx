#include "json_parser.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

namespace gfx::utils
{
JSONParser::JSONParser(const std::filesystem::path& filepath)
{
  assert(filepath.extension() == ".json");
  std::ifstream inputstream{filepath};

  nlohmann::json jsonObject{};

  inputstream >> jsonObject;

  std::cout << jsonObject << '\n';
}
} // namespace gfx::utils
