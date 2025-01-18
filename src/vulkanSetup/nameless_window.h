#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace nameless {
	class NamelessWindow {
	public:
		NamelessWindow(int w, int h, std::string name);
		~NamelessWindow();

		NamelessWindow(const NamelessWindow&) = delete;
		NamelessWindow& operator=(const NamelessWindow&) = delete;

		bool shouldClose() {
			return glfwWindowShouldClose(window);
		}
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void initWindow(); 
		const int height;
		const int width;

		std::string windowName;
		GLFWwindow* window;
	};

}


