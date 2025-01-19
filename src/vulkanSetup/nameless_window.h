#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <functional>

namespace nameless {
	class NamelessWindow {
	public:
		NamelessWindow(int w, int h, std::string name);
		~NamelessWindow();

		NamelessWindow(const NamelessWindow&) = delete;
		NamelessWindow& operator=(const NamelessWindow&) = delete;

		bool wasWindowResized() { return frameBufferResized; }
		void resetWindowResizedFlag() { frameBufferResized = false; }

		bool shouldClose() {
			return glfwWindowShouldClose(window);
		}
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);


		/*To support dynamic resizing of the window, we push the lambda function from app::run()
		* which does three necessary things:
		* 1. reset the window resized flag to false
		* 2. recreate the swapchain with the new window size
		* 3. draw the needed frame.
		* 
		* Added for dynamic resizing on windows to provide a consistently correct image without freezing.
		*/
		void subscribe(const std::function<void()>& callback) {
			subscribers.push_back(callback);
		}

		void notifySubscribers() {
			for (const auto &subscriber : subscribers) {
				subscriber();
			}
		}
	private:
		static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);
		std::vector<std::function<void()>> subscribers;
		void initWindow(); 
		int height;
		int width;
		bool frameBufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};

}


