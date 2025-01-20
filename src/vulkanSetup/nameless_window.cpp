#include "nameless_window.h"
#include <stdexcept>

namespace nameless {

	NamelessWindow::NamelessWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
		initWindow();
	}

	NamelessWindow::~NamelessWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	void NamelessWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void NamelessWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

	void NamelessWindow::frameBufferResizeCallback(GLFWwindow *window, int width, int height) {
		auto namelessWindow = reinterpret_cast<NamelessWindow*>(glfwGetWindowUserPointer(window));
		namelessWindow->frameBufferResized = true;
		namelessWindow->height = height;
		namelessWindow->width = width;
	}
}