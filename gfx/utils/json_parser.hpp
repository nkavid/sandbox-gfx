#pragma once

#include <filesystem>

namespace gfx::utils
{
class JSONParser
{
    explicit JSONParser(const std::filesystem::path& filepath);
};
}
