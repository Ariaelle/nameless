#include <stdexcept>

#include "nameless_renderer.h"


namespace nameless {
	NamelessRenderer::NamelessRenderer(NamelessWindow &window, NamelessDevice &device) : namelessWindow{ window }, namelessDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}
	NamelessRenderer::~NamelessRenderer() {
		freeCommandBuffers();
	}

	void NamelessRenderer::createCommandBuffers() {
		commandBuffers.resize(NamelessSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = namelessDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(namelessDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void NamelessRenderer::recreateSwapChain() {
		auto extent = namelessWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = namelessWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(namelessDevice.device());
		if (namelessSwapChain == nullptr) {
			namelessSwapChain = std::make_unique<NamelessSwapChain>(namelessDevice, extent);
		}
		else {
			std::shared_ptr<NamelessSwapChain> oldSwapChain = std::move(namelessSwapChain);
			namelessSwapChain = std::make_unique<NamelessSwapChain>(namelessDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*namelessSwapChain.get())) {
				throw std::runtime_error("Swap chain image format has changed");
			}
		}
		//we'll come back to this and redo a thing
	}


	void NamelessRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(namelessDevice.device(), namelessDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer NamelessRenderer::beginFrame(){
		assert(!isFrameStarted && "Can't begin a frame while one is in progress.");

		auto result = namelessSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire next swapchain img");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording cmd buffer");
		}

		return commandBuffer;

	}
	void NamelessRenderer::endFrame() {
		assert(isFrameStarted && "Can't call end frame with no frame in progress.");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		auto result = namelessSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || namelessWindow.wasWindowResized()) {
			namelessWindow.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain img");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % NamelessSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void NamelessRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on a command buffer from a different frame.");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = namelessSwapChain->getRenderPass();
		renderPassInfo.framebuffer = namelessSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = namelessSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(namelessSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(namelessSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, namelessSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	}
	void NamelessRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on a command buffer from a different frame.");

		vkCmdEndRenderPass(commandBuffer);
	}
}