#include "ZCUDARenderer.h"

void ZCUDARenderer::Run()
{
    initWindow();
    mainLoop();
}

void ZCUDARenderer::initWindow()
{
	mWindow = std::make_shared<platform::Window>(mWidth, mHeight);

    // TODO 编写并注册回调函数
}

void ZCUDARenderer::mainLoop()
{
    double fpsTimer = 0.0f;
    int    frameCount = 0;
    while (!mWindow->shouldClose())
    {
        mWindow->pollEvents();
        mWindow->processEvents();

        float dt = getFrameTime();

        fpsTimer += dt;
        frameCount++;

        if (fpsTimer >= 1.0)
        {
            std::string title = "ZCudaRenderer | FPS: " + std::to_string(static_cast<int>(frameCount / fpsTimer));

            glfwSetWindowTitle(mWindow->getWindow(), title.c_str());

            fpsTimer = 0.0;
            frameCount = 0;
        }

        //mScene->update(dt, mCurrentFrame);

        //drawFrame();
    }
    //vkDeviceWaitIdle(mDevice->getDevice());
}

float ZCUDARenderer::getFrameTime()
{
	static double lastTime = 0.0f;
	double        currentTime = glfwGetTime();
	float         deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    return deltaTime;
}
