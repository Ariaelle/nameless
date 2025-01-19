#pragma once

#include "vulkanSetup/nameless_window.h"
#include "vulkanSetup/nameless_pipeline.h"
#include "vulkanSetup/nameless_device.h"
#include "vulkanSetup/nameless_swapchain.h"
#include "vulkanSetup/nameless_model.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <array>

namespace nameless {
	class app {

	public:
		static constexpr int width = 1280;
		static constexpr int height = 720;
		
		app();
		~app();

		app(const app&) = delete;
		app& operator=(const app&) = delete;
		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void recordCommandBuffer(int imageIndex);
		void drawFrame();
		void recreateSwapChain();

		NamelessWindow namelessWindow{ width, height, std::string("Window")};
		NamelessDevice namelessDevice{ namelessWindow };
		std::unique_ptr<NamelessSwapChain> namelessSwapChain;
		std::unique_ptr<NamelessPipeline> namelessPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<NamelessModel> namelessModel;
	};
}