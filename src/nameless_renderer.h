#pragma once

#include "vulkanSetup/nameless_window.h"
#include "vulkanSetup/nameless_device.h"
#include "vulkanSetup/nameless_swapchain.h"
#include "vulkanSetup/nameless_model.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <array>
#include <cassert>

namespace nameless {
	class NamelessRenderer {

	public:

		NamelessRenderer(NamelessWindow & window, NamelessDevice &device);
		~NamelessRenderer();

		NamelessRenderer(const NamelessRenderer&) = delete;
		NamelessRenderer& operator=(const NamelessRenderer&) = delete;

		bool isFrameInProgress() const { return isFrameStarted; }

		VkRenderPass getSwapChainRenderPass() const { return namelessSwapChain->getRenderPass(); }

		float getAspectRatio() const { return namelessSwapChain->extentAspectRatio(); }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index with no frame in progress");
			return currentFrameIndex;
		}

		VkFormat getSwapChainImageFormat() {
			return namelessSwapChain->getSwapChainImageFormat();
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();


		NamelessWindow &namelessWindow;
		NamelessDevice &namelessDevice;
		std::unique_ptr<NamelessSwapChain> namelessSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}