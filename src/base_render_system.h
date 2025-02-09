#pragma once

#include "vulkanSetup/nameless_pipeline.h"
#include "vulkanSetup/nameless_device.h"
#include "vulkanSetup/nameless_model.h"
#include "nameless_game_object.h"
#include "nameless_camera.h"
#include "nameless_frame_info.h"

#include <vulkan/vulkan.h>
#include <memory>
#include <array>

namespace nameless {
	class BaseRenderSystem {

	public:
		BaseRenderSystem(NamelessDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~BaseRenderSystem();

		BaseRenderSystem(const BaseRenderSystem&) = delete;
		BaseRenderSystem& operator=(const BaseRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		
		NamelessDevice &namelessDevice;
		std::unique_ptr<NamelessPipeline> namelessPipeline;
		VkPipelineLayout pipelineLayout;

	};
}