#pragma once

#include <vulkan/vulkan_core.h>

#include <filesystem>
#include <vector>

namespace gfx::vulkan
{
struct Shader
{
    static std::vector<char> readFile(const std::filesystem::path& filename);

    static VkShaderModule createShaderModule(const std::vector<char>& code,
                                             const VkDevice& device);
};
}
