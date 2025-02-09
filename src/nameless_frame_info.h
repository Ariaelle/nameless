#pragma once
#include "nameless_camera.h"
#include <vulkan/vulkan.h>
#include "nameless_game_object.h"

namespace nameless {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NamelessCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		NamelessGameObject::Map &gameObjects;
	};
}