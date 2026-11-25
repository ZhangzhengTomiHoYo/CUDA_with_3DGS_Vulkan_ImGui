#pragma once
#include <optional>

#include "../Common.h"
#include "Instance.h"
#include "Surface.h"

namespace vulkancore
{

class Device
{
  public:
    Device(std::shared_ptr<Instance> instance, std::shared_ptr<Surface> surface);
    ~Device();
 
    VkDevice                getDevice() const { return mDevice; }
    VkPhysicalDevice        getPhysicalDevice() const { return mPhysicalDevice; }
    VkQueue                 getGraphicQueue() const { return mGraphicQueue; }
    VkQueue                 getPresentQueue() const { return mPresentQueue; }
    std::optional<uint32_t> getGraphicQueueFamily() const { return mGraphicQueueFamily; }
    std::optional<uint32_t> getPresentQueueFamily() const { return mPresentQueueFamily; }

    // 颜色和深度帧缓冲区均支持的最高 MSAA 采样数
    VkSampleCountFlagBits getMaxUsableSampleCount() const;

  private:
    VkDevice         mDevice         = VK_NULL_HANDLE; // Core !!!
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE; // Core !!!

    std::shared_ptr<Instance> mInstance = nullptr;
    std::shared_ptr<Surface>  mSurface  = nullptr;

    std::optional<uint32_t> mGraphicQueueFamily;
    std::optional<uint32_t> mPresentQueueFamily;
    VkQueue                 mGraphicQueue = VK_NULL_HANDLE;
    VkQueue                 mPresentQueue = VK_NULL_HANDLE;

    const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME};
    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

  private:
    void choosePhysicalDevice();
    void initQueueFamilies(VkPhysicalDevice device);
    bool isQueueFamilyComplete();
    void createLogicalDevice();
};

} // namespace vulkancore