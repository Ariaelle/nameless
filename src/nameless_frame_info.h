#pragma once
#include "nameless_camera.h"
#include <vulkan/vulkan.h>
#include "nameless_game_object.h"

namespace nameless {
#define MAX_LIGHTS 10

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NamelessCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		NamelessGameObject::Map &gameObjects;
	};


	struct PointLight {
		glm::vec4 position{}; //ignore w
		glm::vec4 color{}; //w is intensity
	};

	struct GlobalUniformBufferObject {
		alignas(16) glm::mat4 projectionMatrix{ 1.f };
		alignas(16) glm::mat4 viewMatrix{ 1.f };
		//alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, 3.f, -1.f });

		alignas(16) glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};


}