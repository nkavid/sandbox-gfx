#include "context.hpp"

#include "utils/logger.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <cstring>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace gfx::vulkan::detail
{
// NOLINTNEXTLINE
static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
  uint32_t extensionCount{};
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions{extensionCount};

  vkEnumerateDeviceExtensionProperties(device,
                                       nullptr,
                                       &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const auto& extension : availableExtensions)
  {
    requiredExtensions.erase(extension.extensionName); // NOLINT
  }

  return requiredExtensions.empty();
}

static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                            VkSurfaceKHR surface)
{
  QueueFamilyIndices queueFamilyIndices{};
  uint32_t queueFamilyCount{0};
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device,
                                           &queueFamilyCount,
                                           queueFamilies.data());

  uint32_t familyIndex{0}; // NOLINT
  for (const auto& queueFamily : queueFamilies)
  {
    if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0U)
    {
      queueFamilyIndices.graphicsFamily = familyIndex;
    }

    VkBool32 presentSupport{VK_FALSE};
    vkGetPhysicalDeviceSurfaceSupportKHR(device, familyIndex, surface, &presentSupport);
    if (presentSupport == VK_TRUE)
    {
      queueFamilyIndices.presentFamily = familyIndex;
    }

    if (queueFamilyIndices.isComplete())
    {
      break;
    }

    ++familyIndex;
  }

  return queueFamilyIndices;
}

static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                                     VkSurfaceKHR surface)
{
  SwapChainSupportDetails details{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount{};
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0)
  {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,
                                         surface,
                                         &formatCount,
                                         details.formats.data());
  }

  uint32_t presentModeCount{};
  vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                            surface,
                                            &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0)
  {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                              surface,
                                              &presentModeCount,
                                              details.presentModes.data());
  }

  return details;
}

static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  const QueueFamilyIndices queueFamilyIndices = detail::findQueueFamilies(device,
                                                                          surface);

  const bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported)
  {
    auto swapChainSupport = querySwapChainSupport(device, surface);
    swapChainAdequate     = !swapChainSupport.formats.empty()
                     && !swapChainSupport.presentModes.empty();
  }

  const bool result = (deviceProperties.deviceType
                       == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                   && queueFamilyIndices.isComplete() && extensionsSupported
                   && swapChainAdequate;

  return result;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* /*pUserData*/)
{
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
  {
    utils::logger::error(pCallbackData->pMessage);
    // throw std::runtime_error(std::string{"gfx::ERROR::"} + pCallbackData->pMessage);
  }

  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
  {
    utils::logger::warning(pCallbackData->pMessage);
  }

  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
  {
    utils::logger::info(pCallbackData->pMessage);
  }

  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
  {
    // utils::logger::info(pCallbackData->pMessage);
  }
  return VK_FALSE;
}

static void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
  // NOLINTBEGIN(hicpp-signed-bitwise)
  createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;

  // NOLINTEND(hicpp-signed-bitwise)
}

static VkResult
CreateDebugUtilsMessengerEXT(VkInstance instance,
                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                             const VkAllocationCallbacks* pAllocator,
                             VkDebugUtilsMessengerEXT* pDebugMessenger)
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

  if (func != nullptr)
  {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                          VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks* pAllocator)
{
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

  if (func != nullptr)
  {
    func(instance, debugMessenger, pAllocator);
  }
}
}

namespace gfx::vulkan
{
void Context::initialize()
{
  _initWindow();
  _createInstance();
  _setupDebugMessenger();
  _createSurface();
  _pickPhysicalDevice();
}

QueueFamilyIndices Context::findQueueFamilies()
{
  return detail::findQueueFamilies(_physicalDevice, _surface);
}

SwapChainSupportDetails Context::querySwapChainSupport()
{
  return detail::querySwapChainSupport(_physicalDevice, _surface);
}

VkPhysicalDevice Context::getPhysicalDevice() const
{
  return _physicalDevice;
}

GLFWwindow* Context::getWindow() const
{
  return _window;
}

VkSurfaceKHR Context::getSurface() const
{
  return _surface;
}

bool Context::open() const
{
  glfwPollEvents();
  return glfwWindowShouldClose(_window) == GLFW_FALSE;
}

bool Context::resized() const
{
  return _framebufferResized;
}

void Context::setResized(bool newState)
{
  _framebufferResized = newState;
}

void Context::waitWhileMinimized() const
{
  int width{0};
  int height{0};

  do
  {
    glfwGetFramebufferSize(_window, &width, &height);
    glfwWaitEvents();
  } while (width == 0 || height == 0);
}

void Context::terminate()
{
  if (_enableValidationLayers)
  {
    detail::DestroyDebugUtilsMessengerEXT(_vkInstance, _debugMessenger, nullptr);
  }

  vkDestroySurfaceKHR(_vkInstance, _surface, nullptr);
  vkDestroyInstance(_vkInstance, nullptr);

  glfwDestroyWindow(_window);
  glfwTerminate();
}

void Context::_initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  _window = glfwCreateWindow(static_cast<int>(_width),
                             static_cast<int>(_height),
                             _applicationName,
                             nullptr,
                             nullptr);

  glfwSetWindowUserPointer(_window, this);
  glfwSetFramebufferSizeCallback(_window, _framebufferResizeCallback);
}

std::vector<const char*> Context::_getRequiredExtensions() const
{
  uint32_t glfwExtensionCount{0};
  const char** glfwExtensions{nullptr};
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char*> extensions{};

  for (uint32_t i = 0; i < glfwExtensionCount; ++i)
  {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    extensions.push_back(glfwExtensions[i]);
  }

  if (_enableValidationLayers)
  {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

// NOLINTNEXTLINE(readability-function-size)
bool Context::_checkValidationLayerSupport()
{
  uint32_t layerCount{0};
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const auto& layerName : _validationLayers)
  {
    for (const auto& layerProperties : availableLayers)
    {
      if (strcmp(layerName, static_cast<const char*>(layerProperties.layerName)) == 0)
      {
        return true;
      }
    }
  }

  return false;
}

void Context::_createInstance()
{
  if (_enableValidationLayers && !_checkValidationLayerSupport())
  {
    throw std::runtime_error("gfx::validation layers not available!");
  }

  VkApplicationInfo appInfo{};
  appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName   = _applicationName;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName        = "No Engine";
  appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion         = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  auto extensions                    = _getRequiredExtensions();
  createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (_enableValidationLayers)
  {
    createInfo.enabledLayerCount   = static_cast<uint32_t>(_validationLayers.size());
    createInfo.ppEnabledLayerNames = _validationLayers.data();

    detail::populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = &debugCreateInfo;
  }
  else
  {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext             = nullptr;
  }

  if (vkCreateInstance(&createInfo, nullptr, &_vkInstance) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create instance!");
  }
}

void Context::_setupDebugMessenger()
{
  if (!_enableValidationLayers)
  {
    return;
  }

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  detail::populateDebugMessengerCreateInfo(createInfo);

  if (detail::CreateDebugUtilsMessengerEXT(_vkInstance,
                                           &createInfo,
                                           nullptr,
                                           &_debugMessenger)
      != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to set up debug messenger!");
  }
}

void Context::_createSurface()
{
  if (glfwCreateWindowSurface(_vkInstance, _window, nullptr, &_surface) != VK_SUCCESS)
  {
    throw std::runtime_error("gfx::failed to create window surface!");
  }
}

void Context::_pickPhysicalDevice()
{
  uint32_t deviceCount = 0;

  vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, nullptr);

  if (deviceCount == 0)
  {
    throw std::runtime_error("gfx::failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, devices.data());

  for (const auto& device : devices)
  {
    if (detail::isDeviceSuitable(device, _surface))
    {
      _physicalDevice = device;
      break;
    }
  }

  if (_physicalDevice == VK_NULL_HANDLE)
  {
    throw std::runtime_error("gfx::failed to find a suitable GPU!");
  }
}
}
