#include "Swapchain.h"

namespace vulkancore
{

SwapChain::SwapChain(const std::shared_ptr<Device> &device, const std::shared_ptr<platform::Window> &window, const std::shared_ptr<Surface> &surface, const std::shared_ptr<VkCommandPool> &commandPool) : mDevice(device), mWindow(window), mSurface(surface), mCommandPool(commandPool)
{
    createResources();
}

void SwapChain::createResources()
{
    // === 查询支持情况，并选择交换链配置 ===
    auto info        = querySwapChainSupportInfo();               // 查询支持情况
    auto format      = chooseSurfaceFormat(info.mFormats);        // 图像格式和色彩空间
    auto presentMode = choosePresentMode(info.mPresentModes);     // 呈现模式
    auto extent      = chooseSwapchainExtent(info.mCapabilities); // 尺寸

    mSwapChainFormat = format.format;
    mSwapChainExtent = extent;
    std::cout << "chosen: format=" << static_cast<int>(format.format) << "  presentMode=" << static_cast<int>(presentMode) << "  extent=" << extent.width << 'x' << extent.height << '\n';

    // 最终为 三缓冲
    uint32_t minImageCount = info.mCapabilities.minImageCount + 1;
    if (info.mCapabilities.maxImageCount > 0 && minImageCount > info.mCapabilities.maxImageCount)
        minImageCount = info.mCapabilities.maxImageCount;

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface          = mSurface->getSurface();
    swapchainCreateInfo.minImageCount    = minImageCount;
    swapchainCreateInfo.imageColorSpace  = format.colorSpace;
    swapchainCreateInfo.imageFormat      = format.format;
    swapchainCreateInfo.imageExtent      = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    std::vector<uint32_t> indices = {mDevice->getGraphicQueueFamily().value(), mDevice->getPresentQueueFamily().value()};

    if (indices[0] == indices[1])
    {
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices   = nullptr;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
        swapchainCreateInfo.pQueueFamilyIndices   = indices.data();
    }

    swapchainCreateInfo.preTransform   = info.mCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode    = presentMode;
    swapchainCreateInfo.clipped        = VK_TRUE;

    // vkCreateSwapchainKHR 内部会创建Image 但还没给图像句柄
    VkResult result = vkCreateSwapchainKHR(mDevice->getDevice(), &swapchainCreateInfo, nullptr, &mSwapChain);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("vkCreateSwapchainKHR failed.");
    }

    vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
    mSwapChainImages.resize(mImageCount);
    vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChainImages.data()); // 获取已经存在的图像句柄

    std::cout << "Swapchain created: " << mSwapChainExtent.width << 'x' << mSwapChainExtent.height << " actual images = " << mImageCount << '\n';

    // ↑所以是: 创建交换链 → 查询实际图像数量 → 调整数组大小 → 获取已有图像的句柄

    // === 创建图像视图 ===
    mSwapChainImageViews.resize(mImageCount);
    for (size_t i = 0; i < mImageCount; i++)
    {
        mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    // 作为一个 3DGS CUDA 渲染器，Vulkan所需的交换链与图像视图已经创建完毕 无须创建深度模板图像
}

void SwapChain::releaseResources()
{
    for (auto framebuffer : mSwapChainFramebuffers)
        vkDestroyFramebuffer(mDevice->getDevice(), framebuffer, nullptr);

    for (auto imageView : mSwapChainImageViews)
        vkDestroyImageView(mDevice->getDevice(), imageView, nullptr);

    if (mSwapChain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
}

SwapChainSupportInfo SwapChain::querySwapChainSupportInfo()
{
    SwapChainSupportInfo info;

    // 这句能检查到窗口尺寸
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);

    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, nullptr);
    if (count)
    {
        info.mFormats.resize(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, info.mFormats.data());
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, nullptr);
    if (count)
    {
        info.mPresentModes.resize(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, info.mPresentModes.data());
    }
    return info;
}

VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
{
    // 像素格式：RGBA各8位 且映射到无符号0~1
    // 颜色空间：sRGB
    for (const auto &surfaceFormat : formats)
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            return surfaceFormat;
    return formats[0];
}

VkPresentModeKHR SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR> &modes)
{
    // 呈现模式：MailBox 只保留最新提交的画面
    for (const auto &presentMode : modes)
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return presentMode;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    int width = 0, height = 0;
    glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);

    VkExtent2D extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    extent.width      = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height     = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

VkImageView SwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image    = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format   = format;

    imageViewCreateInfo.subresourceRange.aspectMask     = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    imageViewCreateInfo.subresourceRange.levelCount     = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount     = 1;

    VkImageView imageView = VK_NULL_HANDLE;
    VkResult    result    = vkCreateImageView(mDevice->getDevice(), &imageViewCreateInfo, nullptr, &imageView);
    if (result != VK_SUCCESS)
        throw std::runtime_error("failed to create image view");

    return imageView;
}

} // namespace vulkancore