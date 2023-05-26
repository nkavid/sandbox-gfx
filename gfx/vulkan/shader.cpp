#include "shader.hpp"

#include <vulkan/vulkan_core.h>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace gfx::vulkan
{
std::vector<char> Shader::readFile(const std::filesystem::path& filename)
{
  std::ifstream file{filename, std::ios::ate | std::ios::binary};

  if (!file.is_open())
  {
    throw std::runtime_error(std::string{"gfx::failed to open file! "}
                             + filename.c_str());
  }
  auto fileSize = file.tellg();
  std::vector<char> buffer(static_cast<size_t>(fileSize));
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

VkShaderModule Shader::createShaderModule(const std::vector<char>& code,
                                          const VkDevice& device)
{
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode    = std::bit_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule{nullptr};
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create shader module!");
  }

  return shaderModule;
}
} // namespace gfx::vulkan
