#pragma once

#include "nameless_renderer.h"
#include "vulkanSetup/nameless_window.h"
#include "vulkanSetup/nameless_device.h"
#include "vulkanSetup/nameless_model.h"
#include "systems/base_render_system.h"
#include "systems/point_light_system.h"
#include "nameless_game_object.h"
#include <vulkan/vulkan.h>
#include <memory>
#include <array>
#include "vulkanSetup/nameless_descriptors.h"


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

		NamelessWindow namelessWindow{ width, height, std::string("Window")};
		NamelessDevice namelessDevice{ namelessWindow };
		NamelessRenderer namelessRenderer{ namelessWindow, namelessDevice };

		std::unique_ptr<NamelessDescriptorPool> globalPool{};
		NamelessGameObject::Map gameObjects;
		
	};
}