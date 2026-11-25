#pragma once

#include <stdexcept>
#include <memory>
#include <string>
#include <iostream>

#include "Platform/Window.h"

class ZCUDARenderer
{
public:
	ZCUDARenderer(int width = 1920, int height = 1080, bool framebufferResized = false) : mWidth(width), mHeight(height), mFramebufferResized(framebufferResized) {}
	~ZCUDARenderer() = default;

	void Run();

private:
	std::shared_ptr<platform::Window> mWindow;

	int mWidth;
	int mHeight;

	bool mFramebufferResized;

private:
	void initWindow();
	void mainLoop();

private:
	float getFrameTime();
};

