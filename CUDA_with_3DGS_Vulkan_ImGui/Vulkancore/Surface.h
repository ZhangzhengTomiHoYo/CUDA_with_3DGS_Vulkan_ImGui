#include "../Common.h"
#include "Instance.h"
#include "../Platform/Window.h"

namespace vulkancore
{

class Surface
{
  public:
    Surface(std::shared_ptr<Instance> instance, std::shared_ptr<platform::Window> window);
    ~Surface();

    VkSurfaceKHR getSurface() const { return mSurface; }
    VkInstance   getInstance() const { return mInstance->getInstance(); }

  private:
    VkSurfaceKHR              mSurface  = VK_NULL_HANDLE; // Core !!!
    std::shared_ptr<Instance> mInstance = nullptr;
}; 


}