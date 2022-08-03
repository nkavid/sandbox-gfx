#pragma once

#include "context.hpp"

#include <vulkan/vulkan_core.h>

#include <array>
#include <cstdint>
#include <vector>

namespace gfx::vulkan
{
class Application
{
  public:
    void initVulkan();
    void mainLoop();
    void terminate();

  private:
    Context _context{};
    VkDevice _logicalDevice{};
    VkQueue _graphicsQueue{};
    VkQueue _presentQueue{};
    VkSwapchainKHR _swapChain{};
    std::vector<VkImage> _swapChainImages{};
    VkFormat _swapChainImageFormat{};
    VkExtent2D _swapChainExtent{};
    std::vector<VkImageView> _swapChainImageViews{};
    VkRenderPass _renderPass{};
    VkDescriptorSetLayout _descriptorSetLayout{};
    VkPipelineLayout _pipelineLayout{};
    VkPipeline _graphicsPipeline{};
    std::vector<VkFramebuffer> _swapChainFramebuffers{};
    VkCommandPool _commandPool{};
    constexpr static const int MAX_FRAMES_IN_FLIGHT{2};
    uint32_t _currentFrame{0};
    VkDescriptorPool _descriptorPool{};
    std::vector<VkDescriptorSet> _descriptorSets{};
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> _commandBuffers{};
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> _imageAvailableSemaphores{};
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> _renderFinishedSemaphores{};
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> _inFlightFences{};
    VkBuffer _vertexBuffer{};
    VkDeviceMemory _vertexBufferMemory{};
    VkBuffer _indexBuffer{};
    VkDeviceMemory _indexBufferMemory{};
    std::vector<VkBuffer> _uniformBuffers{};
    std::vector<VkDeviceMemory> _uniformBuffersMemory{};
    VkImage _textureImage{};
    VkDeviceMemory _textureImageMemory{};
    VkImageView _textureImageView{};
    VkSampler _textureSampler{};
    VkImage _depthImage{};
    VkDeviceMemory _depthImageMemory{};
    VkImageView _depthImageView{};

    void _createLogicalDevice();
    void _createSwapChain();
    void _createImageViews();
    void _createRenderPass();
    void _createDescriptorSetLayout();
    void _createGraphicsPipeline();
    void _createFramebuffers();
    void _createCommandPool();
    VkFormat _findSupportedFormat(const std::vector<VkFormat>& candidates,
                                  VkImageTiling tiling,
                                  VkFormatFeatureFlags features);
    VkFormat _findDepthFormat();
    void _createDepthResources();
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void _createImage(uint32_t width,
                      uint32_t height,
                      VkFormat format,
                      VkImageTiling tiling,
                      VkImageUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkImage& image,
                      VkDeviceMemory& imageMemory);
    void _createTextureImage();
    uint32_t _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkImageView _createImageView(VkImage image,
                                 VkFormat format,
                                 VkImageAspectFlags aspectFlags);
    void _createTextureImageView();
    void _createTextureSampler();
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void _createBuffer(VkDeviceSize size,
                       VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags properties,
                       VkBuffer& buffer,
                       VkDeviceMemory& bufferMemory);
    void _createVertexBuffer();
    void _createIndexBuffer();
    void _createUniformBuffers();
    void _updateUniformBuffer(uint32_t currentImage);
    VkCommandBuffer _beginSingleTimeCommands();
    void _endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void _copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void _transitionImageLayout(VkImage image,
                                VkFormat format,
                                VkImageLayout oldLayout,
                                VkImageLayout newLayout);
    void
    _copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void _createDescriptorPool();
    void _createDescriptorSets();
    void _createCommandBuffers();
    void _recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void _drawFrame();
    void _createSyncObjects();
    void _cleanupSwapChain();
    void _recreateSwapChain();
};
}
