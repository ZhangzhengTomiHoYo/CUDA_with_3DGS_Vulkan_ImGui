#pragma once
#include "../Common.h"
#include "../Platform/Window.h"
#include "Device.h"

namespace vulkancore
{
class RenderPass; // 前向声明
}

namespace vulkancore
{

struct SwapChainSupportInfo
{
    VkSurfaceCapabilitiesKHR        mCapabilities = {};
    std::vector<VkSurfaceFormatKHR> mFormats;
    std::vector<VkPresentModeKHR>   mPresentModes;
};

class SwapChain
{
  public:
    SwapChain(const std::shared_ptr<Device> &device, const std::shared_ptr<platform::Window> &window, const std::shared_ptr<Surface> &surface, const std::shared_ptr<VkCommandPool> &commandPool);
    ~SwapChain();

    void createFrameBuffers(const std::shared_ptr<RenderPass> &renderPass);

    VkFormat                   getSwapChainImageFormat() const { return mSwapChainFormat; }
    VkExtent2D                 getSwapChainExtent() const { return mSwapChainExtent; }
    VkSwapchainKHR             getSwapChain() const { return mSwapChain; }
    uint32_t                   getImageCount() const { return mImageCount; }
    std::vector<VkImage>       getSwapChainImages() const { return mSwapChainImages; }
    std::vector<VkImageView>   getSwapChainImageViews() const { return mSwapChainImageViews; }
    std::vector<VkFramebuffer> getSwapChainFramebuffers() const { return mSwapChainFramebuffers; }

  private:
    VkSwapchainKHR mSwapChain       = VK_NULL_HANDLE; // Core !!!
    VkFormat       mSwapChainFormat = {};
    VkExtent2D     mSwapChainExtent = {};
    uint32_t       mImageCount      = 0; // 交换链图像数量

    std::shared_ptr<Device>           mDevice      = nullptr;
    std::shared_ptr<platform::Window> mWindow      = nullptr;
    std::shared_ptr<Surface>          mSurface     = nullptr;
    std::shared_ptr<VkCommandPool>    mCommandPool = nullptr;

    std::vector<VkImage>                  mSwapChainImages       = {};
    std::vector<std::shared_ptr<VkImage>> mDepthImages           = {};
    std::vector<VkImageView>              mSwapChainImageViews   = {};
    std::vector<VkFramebuffer>            mSwapChainFramebuffers = {};

  private:
    void createResources();
    void recreateSwapchain();
    void releaseResources();

    SwapChainSupportInfo querySwapChainSupportInfo();

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
    VkPresentModeKHR   choosePresentMode(const std::vector<VkPresentModeKHR> &modes);
    VkExtent2D         chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    VkImageView        createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1);



};

} // namespace vulkancore