#include "Surface.h"

namespace vulkancore
{

Surface::Surface(std::shared_ptr<Instance> instance, std::shared_ptr<platform::Window> window) : mInstance(instance)
{
    VkResult result = glfwCreateWindowSurface(instance->getInstance(), window->getWindow(), nullptr, &mSurface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

Surface::~Surface()
{
    if (mSurface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(mInstance->getInstance(), mSurface, nullptr);
    }
}

}