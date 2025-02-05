#pragma once

#include "vulkanSetup/nameless_pipeline.h"
#include "vulkanSetup/nameless_device.h"
#include "vulkanSetup/nameless_model.h"
#include "nameless_game_object.h"
#include "nameless_camera.h"

#include <vulkan/vulkan.h>
#include <memory>
#include <array>

namespace nameless {
	class BaseRenderSystem {

	public:
		BaseRenderSystem(NamelessDevice& device, VkRenderPass renderPass);
		~BaseRenderSystem();

		BaseRenderSystem(const BaseRenderSystem&) = delete;
		BaseRenderSystem& operator=(const BaseRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<NamelessGameObject> &gameObjects, const NamelessCamera& camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		
		NamelessDevice &namelessDevice;
		std::unique_ptr<NamelessPipeline> namelessPipeline;
		VkPipelineLayout pipelineLayout;

	};
}