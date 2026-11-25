#include "Instance.h"

#include <iostream>

namespace vulkancore
{

// 一个静态方法 VKAPI_ATTR VKAPI_CALL 为Vulkan提供的平台宏
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pMessageData, void *pUserData)
{
    const char *level = "DEBUG";

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        level = "ERROR";
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        level = "WARNING";
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        level = "INFO";

    std::cerr << "[ValidationLayer][" << level << "] " << pMessageData->pMessage << '\n';

    return VK_FALSE;
}

// 一个静态方法 用于销毁调试对象
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    // 获取销毁函数的地址
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        // 调用它 此时才真正销毁调式对象
        func(instance, debugMessenger, pAllocator);
    }
}

// 一个静态方法 用于创建调试信息
static VkResult CreateDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *debugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, debugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

Instance::Instance(bool enableValidationLayer) 
{
    if (mEnableValidationLayer && !checkValidationLayerSupport())
    {
        throw std::runtime_error("Erro: validation layer is not supported.");
    }

    uint32_t supportedVersion = VK_API_VERSION_1_0;
    vkEnumerateInstanceVersion(&supportedVersion);
    if (supportedVersion < VK_API_VERSION_1_4)
    {
        throw std::runtime_error("Error: Vulkan 1.4 not supported on this system");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "ZCudaRenderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "NO ENGINE";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = VK_API_VERSION_1_4;

    auto extensions = getRequiredExtensions();

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo        = &appInfo;
    instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    if (mEnableValidationLayer)
    {
        instanceCreateInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Error: failed to create instance");
    }
    setupDebugger();
}
Instance::~Instance()
{
    if (mEnableValidationLayer)
    {
        DestroyDebugUtilsMessengerEXT(mInstance, mDebugger, nullptr);
    }
    vkDestroyInstance(mInstance, nullptr);
}
void Instance::setupDebugger()
{
    if (!mEnableValidationLayer)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallBack;

    VkResult result = CreateDebugUtilsMessengerExt(mInstance, &createInfo, nullptr, &mDebugger);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Error: failed to create debugger");
    }
}

bool Instance::checkValidationLayerSupport()
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto &layerName : validationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProp : availableLayers)
        {
            if (std::strcmp(layerProp.layerName, layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }

    return true;
}

std::vector<const char *> Instance::getRequiredExtensions()
{
    uint32_t     glfwExtCount = 0;
    const char **glfwExts     = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    std::vector<const char *> extensions(glfwExts, glfwExts + glfwExtCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}
} // namespace vulkancore