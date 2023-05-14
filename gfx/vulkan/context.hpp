#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include <bit>
#include <cstdint>
#include <optional>
#include <vector>

namespace gfx::vulkan
{
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily{};
    std::optional<uint32_t> presentFamily{};

    [[nodiscard]] bool isComplete() const
    {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats{};
    std::vector<VkPresentModeKHR> presentModes{};
};

class Context
{
  public:
    void initialize();
    QueueFamilyIndices findQueueFamilies();
    SwapChainSupportDetails querySwapChainSupport();
    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const;
    [[nodiscard]] GLFWwindow* getWindow() const;
    [[nodiscard]] VkSurfaceKHR getSurface() const;
    [[nodiscard]] bool open() const;
    [[nodiscard]] bool resized() const;
    void setResized(bool newState);
    void waitWhileMinimized() const;
    void terminate();

  private:
    GLFWwindow* _window{nullptr};
    // NOLINTBEGIN(readability-magic-numbers)
    uint32_t _width{800};
    uint32_t _height{600};
    // NOLINTEND(readability-magic-numbers)
    VkInstance _vkInstance{};
    const char* _applicationName{"vulkan-hello-application"};
    VkDebugUtilsMessengerEXT _debugMessenger{};
    VkPhysicalDevice _physicalDevice{VK_NULL_HANDLE};
    bool _framebufferResized{false};
    bool _enableValidationLayers{true};
    std::vector<const char*> _validationLayers{"VK_LAYER_KHRONOS_validation"};
    VkSurfaceKHR _surface{};

    [[nodiscard]] std::vector<const char*> _getRequiredExtensions() const;
    bool _checkValidationLayerSupport();
    void _initWindow();
    void _createInstance();
    void _setupDebugMessenger();
    void _createSurface();
    void _pickPhysicalDevice();

    static void _framebufferResizeCallback(GLFWwindow* window,
                                           int /*width*/,
                                           int /*height*/)
    {
      auto* app = std::bit_cast<Context*>(glfwGetWindowUserPointer(window));
      app->_framebufferResized = true;
    }
};
}
