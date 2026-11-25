#include <iostream>
#include <Windows.h>

#include "ZCUDARenderer.h"

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	std::cout << "[INFO]  Hello CUDA_with_3DGS_Vulkan_ImGui!" << std::endl;

	ZCUDARenderer renderer;

	try {
		renderer.Run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "[ERROR] 渲染器异常: "<< e.what() << std::endl;
	}
}