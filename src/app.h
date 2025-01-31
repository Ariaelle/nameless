#pragma once

#include "nameless_renderer.h"
#include "vulkanSetup/nameless_window.h"
#include "vulkanSetup/nameless_pipeline.h"
#include "vulkanSetup/nameless_device.h"
#include "vulkanSetup/nameless_model.h"
#include "nameless_game_object.h"
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
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void renderGameObjects(VkCommandBuffer commandBuffer);

		NamelessWindow namelessWindow{ width, height, std::string("Window")};
		NamelessDevice namelessDevice{ namelessWindow };
		NamelessRenderer namelessRenderer{ namelessWindow, namelessDevice };
		std::unique_ptr<NamelessPipeline> namelessPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<NamelessGameObject> gameObjects;
		
	};
}