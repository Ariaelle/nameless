#include "app.h"
#include <stdexcept>

namespace nameless {

	app::app() {

		//See the comment in nameless_window.h
		namelessWindow.subscribe([this]() {
			namelessWindow.resetWindowResizedFlag(); recreateSwapChain(); drawFrame(); });


		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}
	app::~app() {
		vkDestroyPipelineLayout(namelessDevice.device(), pipelineLayout, nullptr);

	}
	void app::run() {
		while (!namelessWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}
		vkDeviceWaitIdle(namelessDevice.device());
	}

	void app::loadModels() {
		std::vector<NamelessModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f} , {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		namelessModel = std::make_unique<NamelessModel>(namelessDevice, vertices);
	}

	void app::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(namelessDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}
	void app::createPipeline() {
		PipelineConfigInfo pipelineConfig{};
		NamelessPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = namelessSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		namelessPipeline = std::make_unique<NamelessPipeline>(namelessDevice, "shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv", pipelineConfig);

	}

	void app::createCommandBuffers() {
		commandBuffers.resize(namelessSwapChain->imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = namelessDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(namelessDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void app::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording cmd buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = namelessSwapChain->getRenderPass();
		renderPassInfo.framebuffer = namelessSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = namelessSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(namelessSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(namelessSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, namelessSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		namelessPipeline->bind(commandBuffers[imageIndex]);
		namelessModel->bind(commandBuffers[imageIndex]);
		namelessModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void app::recreateSwapChain() {
		auto extent = namelessWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = namelessWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(namelessDevice.device());
		namelessSwapChain = nullptr;
		namelessSwapChain = std::make_unique<NamelessSwapChain>(namelessDevice, extent);
		createPipeline();
	}

	void app::drawFrame() {
		uint32_t imageIndex;
		auto result = namelessSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire next swapchain img");
		}
		recordCommandBuffer(imageIndex);
		result = namelessSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || namelessWindow.wasWindowResized()) {
			namelessWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain img");
		}
	}
}