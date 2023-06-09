#include "application.hpp"

#include "context.hpp"
#include "shader.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <vulkan/vulkan_core.h>

#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace gfx::vulkan::detail
{
namespace
{
VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
  for (const auto& availableFormat : availableFormats)
  {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
        && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
  for (const auto& availablePresentMode : availablePresentModes)
  {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                            GLFWwindow* window)
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    return capabilities.currentExtent;
  }

  int width{};
  int height{};

  glfwGetFramebufferSize(window, &width, &height);

  VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                             static_cast<uint32_t>(height)};

  actualExtent.width = std::clamp(actualExtent.width,
                                  capabilities.minImageExtent.width,
                                  capabilities.maxImageExtent.width);

  actualExtent.height = std::clamp(actualExtent.height,
                                   capabilities.minImageExtent.height,
                                   capabilities.maxImageExtent.height);

  return actualExtent;
}
} // namespace
} // namespace gfx::vulkan::detail

namespace gfx::vulkan
{
namespace attribute
{
enum Attribute
{
  position,
  color,
  texCoord,
  num_attributes
};
} // namespace attribute

struct Vertex
{
    glm::vec3 position{};
    glm::vec3 color{};
    glm::vec2 texCoord{};

    static VkVertexInputBindingDescription getBindingDescription()
    {
      VkVertexInputBindingDescription bindingDescription{};

      bindingDescription.binding   = 0;
      bindingDescription.stride    = sizeof(Vertex);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
    }

    using AttributeDescriptionArray =
        std::array<VkVertexInputAttributeDescription, attribute::num_attributes>;

    static AttributeDescriptionArray getAttributeDescriptions()
    {
      AttributeDescriptionArray attributeDescriptions{};

      attributeDescriptions[attribute::position].binding  = 0;
      attributeDescriptions[attribute::position].location = attribute::position;
      attributeDescriptions[attribute::position].format   = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[attribute::position].offset   = offsetof(Vertex, position);

      attributeDescriptions[attribute::color].binding  = 0;
      attributeDescriptions[attribute::color].location = attribute::color;
      attributeDescriptions[attribute::color].format   = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[attribute::color].offset   = offsetof(Vertex, color);

      attributeDescriptions[attribute::texCoord].binding  = 0;
      attributeDescriptions[attribute::texCoord].location = attribute::texCoord;
      attributeDescriptions[attribute::texCoord].format   = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[attribute::texCoord].offset   = offsetof(Vertex, texCoord);

      return attributeDescriptions;
    }
};

constexpr std::array<uint16_t, 12> indices{0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

constexpr static const size_t VEC4_ALIGNMENT{16};

struct UniformBufferObject
{
    alignas(VEC4_ALIGNMENT) glm::mat4 model{};
    alignas(VEC4_ALIGNMENT) glm::mat4 view{};
    alignas(VEC4_ALIGNMENT) glm::mat4 proj{};
};

void Application::initVulkan()
{
  _context.initialize();
  _createLogicalDevice();
  _createSwapChain();
  _createImageViews();
  _createRenderPass();
  _createDescriptorSetLayout();
  _createGraphicsPipeline();
  _createCommandPool();
  _createDepthResources();
  _createFramebuffers();
  _createTextureImage();
  _createTextureImageView();
  _createTextureSampler();
  _createVertexBuffer();
  _createIndexBuffer();
  _createUniformBuffers();
  _createDescriptorPool();
  _createDescriptorSets();
  _createCommandBuffers();
  _createSyncObjects();
}

void Application::mainLoop()
{
  while (_context.open())
  {
    _drawFrame();
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::terminate()
{
  vkDeviceWaitIdle(_logicalDevice);

  vkDestroySampler(_logicalDevice, _textureSampler, nullptr);
  vkDestroyImageView(_logicalDevice, _textureImageView, nullptr);
  vkDestroyImage(_logicalDevice, _textureImage, nullptr);
  vkFreeMemory(_logicalDevice, _textureImageMemory, nullptr);

  vkDestroyBuffer(_logicalDevice, _indexBuffer, nullptr);
  vkFreeMemory(_logicalDevice, _indexBufferMemory, nullptr);

  vkDestroyBuffer(_logicalDevice, _vertexBuffer, nullptr);
  vkFreeMemory(_logicalDevice, _vertexBufferMemory, nullptr);

  _cleanupSwapChain();

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
  {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
    vkDestroyFence(_logicalDevice, _inFlightFences[i], nullptr);
    vkDestroySemaphore(_logicalDevice, _renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(_logicalDevice, _imageAvailableSemaphores[i], nullptr);
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  vkDestroyCommandPool(_logicalDevice, _commandPool, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
    vkDestroyBuffer(_logicalDevice, _uniformBuffers[i], nullptr);
    vkFreeMemory(_logicalDevice, _uniformBuffersMemory[i], nullptr);
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
  }

  vkDestroyDescriptorPool(_logicalDevice, _descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(_logicalDevice, _descriptorSetLayout, nullptr);

  vkDestroyPipeline(_logicalDevice, _graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(_logicalDevice, _pipelineLayout, nullptr);
  vkDestroyRenderPass(_logicalDevice, _renderPass, nullptr);

  vkDestroyDevice(_logicalDevice, nullptr);

  _context.terminate();
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createLogicalDevice()
{
  QueueFamilyIndices queueFamilyIndices = _context.findQueueFamilies();

  assert(queueFamilyIndices.graphicsFamily.has_value());
  assert(queueFamilyIndices.presentFamily.has_value());

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
  const std::set<uint32_t> uniqueQueueFamilies =
      {queueFamilyIndices.graphicsFamily.value(),
       queueFamilyIndices.presentFamily.value()};

  const float queuePriority = 1.0F;
  for (const uint32_t queueFamily : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkDeviceCreateInfo createInfo{};
  createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos    = queueCreateInfos.data();
  createInfo.queueCreateInfoCount = 1;

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;
  createInfo.pEnabledFeatures      = &deviceFeatures;

  const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(_context.getPhysicalDevice(),
                     &createInfo,
                     nullptr,
                     &_logicalDevice)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create logical device!");
  }

  vkGetDeviceQueue(_logicalDevice,
                   queueFamilyIndices.graphicsFamily.value(),
                   0,
                   &_graphicsQueue);

  vkGetDeviceQueue(_logicalDevice,
                   queueFamilyIndices.presentFamily.value(),
                   0,
                   &_presentQueue);
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createSwapChain()
{
  auto swapChainSupport = _context.querySwapChainSupport();

  const VkSurfaceFormatKHR surfaceFormat =
      detail::chooseSwapSurfaceFormat(swapChainSupport.formats);

  const VkPresentModeKHR presentMode =
      detail::chooseSwapPresentMode(swapChainSupport.presentModes);

  const VkExtent2D extent = detail::chooseSwapExtent(swapChainSupport.capabilities,
                                                     _context.getWindow());

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

  if ((swapChainSupport.capabilities.maxImageCount > 0)
      && (imageCount > swapChainSupport.capabilities.maxImageCount))
  {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface          = _context.getSurface();
  createInfo.minImageCount    = imageCount;
  createInfo.imageFormat      = surfaceFormat.format;
  createInfo.imageColorSpace  = surfaceFormat.colorSpace;
  createInfo.imageExtent      = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  auto queueFamilyIndices = _context.findQueueFamilies();

  assert(queueFamilyIndices.graphicsFamily.has_value());
  assert(queueFamilyIndices.presentFamily.has_value());

  const std::vector<uint32_t> queueFamilyIndicesData =
      {queueFamilyIndices.graphicsFamily.value(),
       queueFamilyIndices.presentFamily.value()};

  if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily)
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount =
        static_cast<uint32_t>(queueFamilyIndicesData.size());
    createInfo.pQueueFamilyIndices = queueFamilyIndicesData.data();
  }
  else
  {
    createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;       // Optional
    createInfo.pQueueFamilyIndices   = nullptr; // Optional
  }

  createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode    = presentMode;
  createInfo.clipped        = VK_TRUE;
  createInfo.oldSwapchain   = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(_logicalDevice, &createInfo, nullptr, &_swapChain)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(_logicalDevice, _swapChain, &imageCount, nullptr);
  _swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(_logicalDevice,
                          _swapChain,
                          &imageCount,
                          _swapChainImages.data());
  _swapChainImageFormat = surfaceFormat.format;
  _swapChainExtent      = extent;
}

void Application::_createImageViews()
{
  _swapChainImageViews.resize(_swapChainImages.size());

  for (size_t i = 0; i < _swapChainImages.size(); i++)
  {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType                         = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image                         = _swapChainImages[i];
    createInfo.viewType                      = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format                        = _swapChainImageFormat;
    createInfo.components.r                  = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g                  = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b                  = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a                  = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask   = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount   = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount     = 1;

    if (vkCreateImageView(_logicalDevice,
                          &createInfo,
                          nullptr,
                          &_swapChainImageViews[i])
        != VK_SUCCESS)
    {
      throw std::runtime_error("gfx::failed to create image views!");
    }
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format         = _swapChainImageFormat;
  colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format         = _findDepthFormat();
  depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount    = 1;
  subpass.pColorAttachments       = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass   = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass   = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                          | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                          | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                           | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  // NOLINTNEXTLINE(readability-magic-numbers)
  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment,
                                                        depthAttachment};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments    = attachments.data();
  renderPassInfo.subpassCount    = 1;
  renderPassInfo.pSubpasses      = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies   = &dependency;

  if (vkCreateRenderPass(_logicalDevice, &renderPassInfo, nullptr, &_renderPass)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create render pass!");
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createDescriptorSetLayout()
{
  VkDescriptorSetLayoutBinding uboLayoutBinding{};
  uboLayoutBinding.binding            = 0;
  uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount    = 1;
  uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding            = 1;
  samplerLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.descriptorCount    = 1;
  samplerLayoutBinding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
  samplerLayoutBinding.pImmutableSamplers = nullptr;

  // NOLINTNEXTLINE(readability-magic-numbers)
  constexpr unsigned int num_uniforms{2};
  std::array<VkDescriptorSetLayoutBinding, num_uniforms> bindings =
      {uboLayoutBinding, samplerLayoutBinding};

  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings    = bindings.data();

  if (vkCreateDescriptorSetLayout(_logicalDevice,
                                  &layoutInfo,
                                  nullptr,
                                  &_descriptorSetLayout)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create descriptor set layout!");
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createGraphicsPipeline()
{
  auto vertShaderCode = Shader::readFile("gfx/vulkan/shaders/shader.vert.spv");
  auto fragShaderCode = Shader::readFile("gfx/vulkan/shaders/shader.frag.spv");

  VkShaderModule vertShaderModule = Shader::createShaderModule(vertShaderCode,
                                                               _logicalDevice);

  VkShaderModule fragShaderModule = Shader::createShaderModule(fragShaderCode,
                                                               _logicalDevice);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName  = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName  = "main";

  std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {vertShaderStageInfo,
                                                               fragShaderStageInfo};

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates    = dynamicStates.data();

  VkViewport viewport{};
  viewport.x        = 0.0F;
  viewport.y        = 0.0F;
  viewport.width    = static_cast<float>(_swapChainExtent.width);
  viewport.height   = static_cast<float>(_swapChainExtent.height);
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = _swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports    = &viewport;
  viewportState.scissorCount  = 1;
  viewportState.pScissors     = &scissor;

  auto bindDesc = Vertex::getBindingDescription();
  auto attrDesc = Vertex::getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions    = &bindDesc; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attrDesc.size());
  vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data(); // Optional

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable        = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth               = 1.0F;
  rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable         = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0F; // Optional
  rasterizer.depthBiasClamp          = 0.0F; // Optional
  rasterizer.depthBiasSlopeFactor    = 0.0F; // Optional

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable   = VK_FALSE;
  multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading      = 1.0F;     // Optional
  multisampling.pSampleMask           = nullptr;  // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable      = VK_FALSE; // Optional

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  // NOLINTBEGIN(hicpp-signed-bitwise)
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                      | VK_COLOR_COMPONENT_G_BIT
                                      | VK_COLOR_COMPONENT_B_BIT
                                      | VK_COLOR_COMPONENT_A_BIT;
  // NOLINTEND(hicpp-signed-bitwise)
  colorBlendAttachment.blendEnable         = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable   = VK_FALSE;
  colorBlending.logicOp         = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments    = &colorBlendAttachment;

  // NOLINTBEGIN(readability-magic-numbers,clang-diagnostic-unsafe-buffer-usage)
  colorBlending.blendConstants[0] = 0.0F; // Optional
  colorBlending.blendConstants[1] = 0.0F; // Optional
  colorBlending.blendConstants[2] = 0.0F; // Optional
  colorBlending.blendConstants[3] = 0.0F; // Optional
  // NOLINTEND(readability-magic-numbers,clang-diagnostic-unsafe-buffer-usage)

  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable       = VK_TRUE;
  depthStencil.depthWriteEnable      = VK_TRUE;
  depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds        = 0.0F; // Optional
  depthStencil.maxDepthBounds        = 1.0F; // Optional
  depthStencil.stencilTestEnable     = VK_FALSE;
  depthStencil.front                 = {}; // Optional
  depthStencil.back                  = {}; // Optional

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;                     // Optional
  pipelineLayoutInfo.pSetLayouts    = &_descriptorSetLayout; // Optional

  if (vkCreatePipelineLayout(_logicalDevice,
                             &pipelineLayoutInfo,
                             nullptr,
                             &_pipelineLayout)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount          = 2; // NOLINT(readability-magic-numbers)
  pipelineInfo.pStages             = shaderStages.data();
  pipelineInfo.pVertexInputState   = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState      = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState   = &multisampling;
  pipelineInfo.pDepthStencilState  = &depthStencil;
  pipelineInfo.pColorBlendState    = &colorBlending;
  pipelineInfo.pDynamicState       = &dynamicState;
  pipelineInfo.layout              = _pipelineLayout;
  pipelineInfo.renderPass          = _renderPass;
  pipelineInfo.subpass             = 0;
  pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex   = -1;             // Optional

  if (vkCreateGraphicsPipelines(_logicalDevice,
                                VK_NULL_HANDLE,
                                1,
                                &pipelineInfo,
                                nullptr,
                                &_graphicsPipeline)
      != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  vkDestroyShaderModule(_logicalDevice, fragShaderModule, nullptr);
  vkDestroyShaderModule(_logicalDevice, vertShaderModule, nullptr);
}

void Application::_createFramebuffers()
{
  _swapChainFramebuffers.resize(_swapChainImageViews.size());

  for (size_t i = 0; i < _swapChainImageViews.size(); i++)
  {
    constexpr unsigned int num_attachments{2};
    std::array<VkImageView, num_attachments> attachments = {_swapChainImageViews[i],
                                                            _depthImageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass      = _renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments    = attachments.data();
    framebufferInfo.width           = _swapChainExtent.width;
    framebufferInfo.height          = _swapChainExtent.height;
    framebufferInfo.layers          = 1;

    if (vkCreateFramebuffer(_logicalDevice,
                            &framebufferInfo,
                            nullptr,
                            &_swapChainFramebuffers[i])
        != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void Application::_createCommandPool()
{
  auto queueFamilyIndices = _context.findQueueFamilies();

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  assert(queueFamilyIndices.graphicsFamily.has_value());
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(_logicalDevice, &poolInfo, nullptr, &_commandPool)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create command pool!");
  }
}

VkFormat Application::_findSupportedFormat(const std::vector<VkFormat>& candidates,
                                           VkImageTiling tiling,
                                           VkFormatFeatureFlags features)
{
  for (const VkFormat format : candidates)
  {
    VkFormatProperties props{};
    vkGetPhysicalDeviceFormatProperties(_context.getPhysicalDevice(), format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR
        && (props.linearTilingFeatures & features) == features)
    {
      return format;
    }

    if (tiling == VK_IMAGE_TILING_OPTIMAL
        && (props.optimalTilingFeatures & features) == features)
    {
      return format;
    }
  }
  throw std::runtime_error("gfx::failed to find supported format!");
}

VkFormat Application::_findDepthFormat()
{
  return _findSupportedFormat({VK_FORMAT_D32_SFLOAT,
                               VK_FORMAT_D32_SFLOAT_S8_UINT,
                               VK_FORMAT_D24_UNORM_S8_UINT},
                              VK_IMAGE_TILING_OPTIMAL,
                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void Application::_createDepthResources()
{
  const VkFormat depthFormat = _findDepthFormat();

  _createImage(_swapChainExtent.width,
               _swapChainExtent.height,
               depthFormat,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               _depthImage,
               _depthImageMemory);

  _depthImageView = _createImageView(_depthImage,
                                     depthFormat,
                                     VK_IMAGE_ASPECT_DEPTH_BIT);

  _transitionImageLayout(_depthImage,
                         depthFormat,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void Application::_createImage(uint32_t width,
                               uint32_t height,
                               VkFormat format,
                               VkImageTiling tiling,
                               VkImageUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               VkImage& image,
                               VkDeviceMemory& imageMemory)
// NOLINTEND(bugprone-easily-swappable-parameters)
{
  VkImageCreateInfo imageInfo{};
  imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType     = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width  = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth  = 1;
  imageInfo.mipLevels     = 1;
  imageInfo.arrayLayers   = 1;
  imageInfo.format        = format;
  imageInfo.tiling        = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage         = usage;
  imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(_logicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create image!");
  }

  VkMemoryRequirements memRequirements{};
  vkGetImageMemoryRequirements(_logicalDevice, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize  = memRequirements.size;
  allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits,
                                              properties);

  if (vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to allocate image memory!");
  }

  vkBindImageMemory(_logicalDevice, image, imageMemory, 0);
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createTextureImage()
{
  int texWidth{};
  int texHeight{};
  int texChannels{};
  const char* texture_path = "/home/divak/repos/gfx/assets/texture.jpg";

  stbi_uc* pixels =
      stbi_load(texture_path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

  if (pixels == nullptr)
  {
    throw std::runtime_error(std::string{"failed to load texture: "} + texture_path);
  }

  constexpr unsigned int channels{4};
  const VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth * texHeight)
                               * channels;

  VkBuffer stagingBuffer{};
  VkDeviceMemory stagingBufferMemory{};
  _createBuffer(imageSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                    | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

  void* data{};
  vkMapMemory(_logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, imageSize);
  vkUnmapMemory(_logicalDevice, stagingBufferMemory);

  stbi_image_free(pixels);

  _createImage(static_cast<uint32_t>(texWidth),
               static_cast<uint32_t>(texHeight),
               VK_FORMAT_R8G8B8A8_SRGB,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
               _textureImage,
               _textureImageMemory);

  _transitionImageLayout(_textureImage,
                         VK_FORMAT_R8G8B8A8_SRGB,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  _copyBufferToImage(stagingBuffer,
                     _textureImage,
                     static_cast<uint32_t>(texWidth),
                     static_cast<uint32_t>(texHeight));

  _transitionImageLayout(_textureImage,
                         VK_FORMAT_R8G8B8A8_SRGB,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
  vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);
}

uint32_t Application::_findMemoryType(uint32_t typeFilter,
                                      VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(_context.getPhysicalDevice(), &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
  {
    // NOLINTNEXTLINE
    const auto propertyBitField = memProperties.memoryTypes[i].propertyFlags
                                & properties;
    if (((typeFilter & (1U << i)) != 0U) && (propertyBitField == properties))
    {
      return i;
    }
  }

  throw std::runtime_error("gfx::failed to find suitable memory type!");
}

VkImageView Application::_createImageView(VkImage image,
                                          VkFormat format,
                                          VkImageAspectFlags aspectFlags)
{
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image                           = image;
  viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format                          = format;
  viewInfo.subresourceRange.aspectMask     = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel   = 0;
  viewInfo.subresourceRange.levelCount     = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount     = 1;

  VkImageView imageView{};
  if (vkCreateImageView(_logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create texture image view!");
  }

  return imageView;
}

void Application::_createTextureImageView()
{
  _textureImageView = _createImageView(_textureImage,
                                       VK_FORMAT_R8G8B8A8_SRGB,
                                       VK_IMAGE_ASPECT_COLOR_BIT);
}

void Application::_createTextureSampler()
{
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter    = VK_FILTER_LINEAR;
  samplerInfo.minFilter    = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(_context.getPhysicalDevice(), &properties);

  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy    = properties.limits.maxSamplerAnisotropy;

  samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable           = VK_FALSE;
  samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias              = 0.0F;
  samplerInfo.minLod                  = 0.0F;
  samplerInfo.maxLod                  = 0.0F;

  if (vkCreateSampler(_logicalDevice, &samplerInfo, nullptr, &_textureSampler)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create texture sampler!");
  }
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Application::_createBuffer(VkDeviceSize size,
                                VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer& buffer,
                                VkDeviceMemory& bufferMemory)
{
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size        = size;
  bufferInfo.usage       = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(_logicalDevice, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize  = memRequirements.size;
  allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits,
                                              properties);

  if (vkAllocateMemory(_logicalDevice, &allocInfo, nullptr, &bufferMemory)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to allocate buffer memory!");
  }

  vkBindBufferMemory(_logicalDevice, buffer, bufferMemory, 0);
}

void Application::_createVertexBuffer()
{
  constexpr std::array<Vertex, 8> vertices{
      {
       {{-0.5F, -0.5F, 0.0F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
       {{0.5F, -0.5F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F}},
       {{0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}},
       {{-0.5F, 0.5F, 0.0F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F}},
       {{-0.5F, -0.5F, -0.5F}, {1.0F, 0.0F, 0.0F}, {1.0F, 0.0F}},
       {{0.5F, -0.5F, -0.5F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F}},
       {{0.5F, 0.5F, -0.5F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F}},
       {{-0.5F, 0.5F, -0.5F}, {1.0F, 1.0F, 1.0F}, {1.0F, 1.0F}},
       }
  };

  const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

  VkBuffer stagingBuffer{};
  VkDeviceMemory stagingBufferMemory{};
  _createBuffer(bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                    | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

  void* data{};
  vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), bufferSize);
  vkUnmapMemory(_logicalDevice, stagingBufferMemory);

  _createBuffer(bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                _vertexBuffer,
                _vertexBufferMemory);

  _copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

  vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
  vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);
}

void Application::_createIndexBuffer()
{
  const VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

  VkBuffer stagingBuffer{};
  VkDeviceMemory stagingBufferMemory{};
  _createBuffer(bufferSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                    | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

  void* data{};
  vkMapMemory(_logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), bufferSize);
  vkUnmapMemory(_logicalDevice, stagingBufferMemory);

  _createBuffer(bufferSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                _indexBuffer,
                _indexBufferMemory);

  _copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

  vkDestroyBuffer(_logicalDevice, stagingBuffer, nullptr);
  vkFreeMemory(_logicalDevice, stagingBufferMemory, nullptr);
}

void Application::_createUniformBuffers()
{
  const VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
  _uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    _createBuffer(bufferSize,
                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  _uniformBuffers[i],
                  _uniformBuffersMemory[i]);
  }
}

void Application::_updateUniformBuffer(uint32_t currentImage)
{
  // NOLINTBEGIN(readability-magic-numbers)
  static auto startTime = std::chrono::high_resolution_clock::now();

  const auto currentTime = std::chrono::high_resolution_clock::now();
  const float time       = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

  UniformBufferObject ubo{};
  ubo.model = glm::rotate(glm::mat4(1.0F),
                          time * glm::radians(90.0F),
                          glm::vec3(0.0F, 0.0F, 1.0F));

  ubo.view = glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F),
                         glm::vec3(0.0F, 0.0F, 0.0F),
                         glm::vec3(0.0F, 0.0F, 1.0F));

  const float aspectRatio = static_cast<float>(_swapChainExtent.width)
                          / static_cast<float>(_swapChainExtent.height);

  ubo.proj = glm::perspective(glm::radians(45.0F), aspectRatio, 0.1F, 10.0F);

  ubo.proj[1][1] *= -1;

  void* data{};
  vkMapMemory(_logicalDevice,
              _uniformBuffersMemory[currentImage],
              0,
              sizeof(ubo),
              0,
              &data);

  memcpy(data, &ubo, sizeof(ubo));

  vkUnmapMemory(_logicalDevice, _uniformBuffersMemory[currentImage]);
  // NOLINTEND(readability-magic-numbers)
}

VkCommandBuffer Application::_beginSingleTimeCommands()
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool        = _commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer{};
  vkAllocateCommandBuffers(_logicalDevice, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void Application::_endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &commandBuffer;

  vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(_graphicsQueue);

  vkFreeCommandBuffers(_logicalDevice, _commandPool, 1, &commandBuffer);
}

void Application::_copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
  VkCommandBuffer commandBuffer = _beginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  _endSingleTimeCommands(commandBuffer);
}

// NOLINTNEXTLINE(readability-function-size,readability-function-cognitive-complexity)
void Application::_transitionImageLayout(VkImage image,
                                         VkFormat format,
                                         VkImageLayout oldLayout,
                                         VkImageLayout newLayout)
{
  VkCommandBuffer commandBuffer = _beginSingleTimeCommands();

  VkImageMemoryBarrier barrier{};
  barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout           = oldLayout;
  barrier.newLayout           = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image               = image;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
  {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
    {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  }
  else
  {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  barrier.subresourceRange.baseMipLevel   = 0;
  barrier.subresourceRange.levelCount     = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount     = 1;

  VkPipelineStageFlags sourceStage{};
  VkPipelineStageFlags destinationStage{};

  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
      && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
           && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
  {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  }
  else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
           && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                          | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  else
  {
    throw std::invalid_argument("gfx::unsupported layout transition!");
  }

  vkCmdPipelineBarrier(commandBuffer,
                       sourceStage,
                       destinationStage,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       1,
                       &barrier);

  _endSingleTimeCommands(commandBuffer);
}

void Application::_copyBufferToImage(VkBuffer buffer,
                                     VkImage image,
                                     uint32_t width,
                                     uint32_t height)
{
  VkCommandBuffer commandBuffer = _beginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset      = 0;
  region.bufferRowLength   = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel       = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount     = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(commandBuffer,
                         buffer,
                         image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         1,
                         &region);

  _endSingleTimeCommands(commandBuffer);
}

void Application::_createDescriptorPool()
{
  constexpr unsigned int num_uniforms{2};
  std::array<VkDescriptorPoolSize, num_uniforms> poolSizes{};

  poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes    = poolSizes.data();
  poolInfo.maxSets       = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

  if (vkCreateDescriptorPool(_logicalDevice, &poolInfo, nullptr, &_descriptorPool)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create descriptor pool!");
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_createDescriptorSets()
{
  std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                             _descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool     = _descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
  allocInfo.pSetLayouts        = layouts.data();

  _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

  if (vkAllocateDescriptorSets(_logicalDevice, &allocInfo, _descriptorSets.data())
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = _uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range  = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = _textureImageView;
    imageInfo.sampler     = _textureSampler;

    constexpr unsigned int num_uniforms{2};
    std::array<VkWriteDescriptorSet, num_uniforms> descriptorWrites{};

    descriptorWrites[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet          = _descriptorSets[i];
    descriptorWrites[0].dstBinding      = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo     = &bufferInfo;

    descriptorWrites[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet          = _descriptorSets[i];
    descriptorWrites[1].dstBinding      = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo      = &imageInfo;

    vkUpdateDescriptorSets(_logicalDevice,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(),
                           0,
                           nullptr);
  }
}

void Application::_createCommandBuffers()
{
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool        = _commandPool;
  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

  if (vkAllocateCommandBuffers(_logicalDevice, &allocInfo, _commandBuffers.data())
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to allocate command buffers!");
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_recordCommandBuffer(VkCommandBuffer commandBuffer,
                                       uint32_t imageIndex)
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags            = 0;       // Optional
  beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass        = _renderPass;
  renderPassInfo.framebuffer       = _swapChainFramebuffers[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = _swapChainExtent;

  constexpr unsigned int num_attachments{2};
  std::array<VkClearValue, num_attachments> clearValues{};
  clearValues[0].color = {
      {0.0F, 0.0F, 0.0F, 1.0F}
  };
  clearValues[1].depthStencil = {1.0F, 0};

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues    = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

  VkViewport viewport{};
  viewport.x        = 0.0F;
  viewport.y        = 0.0F;
  viewport.width    = static_cast<float>(_swapChainExtent.width);
  viewport.height   = static_cast<float>(_swapChainExtent.height);
  viewport.minDepth = 0.0F;
  viewport.maxDepth = 1.0F;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = _swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  const VkDeviceSize offset{0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertexBuffer, &offset);
  vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
  vkCmdBindDescriptorSets(commandBuffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          _pipelineLayout,
                          0,
                          1,
                          &_descriptorSets[_currentFrame],
                          0,
                          nullptr);

  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to record command buffer!");
  }
}

// NOLINTNEXTLINE(readability-function-size)
void Application::_drawFrame()
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  auto& inFlightFence = _inFlightFences[_currentFrame];
  vkWaitForFences(_logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

  uint32_t imageIndex{};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  auto& imageAvailableSemaphore = _imageAvailableSemaphores[_currentFrame];
  const VkResult acquireResult  = vkAcquireNextImageKHR(_logicalDevice,
                                                       _swapChain,
                                                       UINT64_MAX,
                                                       imageAvailableSemaphore,
                                                       VK_NULL_HANDLE,
                                                       &imageIndex);

  if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
  {
    _recreateSwapChain();
    return;
  }

  if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
  {
    throw std::runtime_error("gfx::failed to acquire swap chain image!");
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  vkResetFences(_logicalDevice, 1, &_inFlightFences[_currentFrame]);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  auto& currentCommandBuffer = _commandBuffers[_currentFrame];
  vkResetCommandBuffer(currentCommandBuffer, 0);
  _recordCommandBuffer(currentCommandBuffer, imageIndex);

  std::vector<VkPipelineStageFlags> waitStages{
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  _updateUniformBuffer(_currentFrame);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  auto& currentRenderFinishedSemaphore = _renderFinishedSemaphores[_currentFrame];
  VkSubmitInfo submitInfo{};
  submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount   = static_cast<uint32_t>(waitStages.size());
  submitInfo.pWaitSemaphores      = &imageAvailableSemaphore;
  submitInfo.pWaitDstStageMask    = waitStages.data();
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &currentCommandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = &currentRenderFinishedSemaphore;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame])
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores    = &currentRenderFinishedSemaphore;
  presentInfo.swapchainCount     = 1;
  presentInfo.pSwapchains        = &_swapChain;
  presentInfo.pImageIndices      = &imageIndex;
  presentInfo.pResults           = nullptr; // Optional

  const VkResult presentResult = vkQueuePresentKHR(_presentQueue, &presentInfo);

  if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR
      || _context.resized())
  {
    _recreateSwapChain();
    _context.setResized(false);
  }
  else if (presentResult != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to present swap chain image!");
  }

  _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::_createSyncObjects()
{
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
  {
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
    if (vkCreateSemaphore(_logicalDevice,
                          &semaphoreInfo,
                          nullptr,
                          &_imageAvailableSemaphores[i])
            != VK_SUCCESS
        || vkCreateSemaphore(_logicalDevice,
                             &semaphoreInfo,
                             nullptr,
                             &_renderFinishedSemaphores[i])
               != VK_SUCCESS
        || vkCreateFence(_logicalDevice, &fenceInfo, nullptr, &_inFlightFences[i])
               != VK_SUCCESS)
    // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)
    {
      throw std::runtime_error("gfx::failed to create semaphores!");
    }
  }
}

void Application::_cleanupSwapChain()
{
  vkDestroyImageView(_logicalDevice, _depthImageView, nullptr);
  vkDestroyImage(_logicalDevice, _depthImage, nullptr);
  vkFreeMemory(_logicalDevice, _depthImageMemory, nullptr);

  for (auto& framebuffer : _swapChainFramebuffers)
  {
    vkDestroyFramebuffer(_logicalDevice, framebuffer, nullptr);
  }

  for (auto& imageView : _swapChainImageViews)
  {
    vkDestroyImageView(_logicalDevice, imageView, nullptr);
  }

  vkDestroySwapchainKHR(_logicalDevice, _swapChain, nullptr);
}

void Application::_recreateSwapChain()
{
  _context.waitWhileMinimized();

  vkDeviceWaitIdle(_logicalDevice);

  _cleanupSwapChain();

  _createSwapChain();
  _createImageViews();
  _createDepthResources();
  _createFramebuffers();
}
} // namespace gfx::vulkan
