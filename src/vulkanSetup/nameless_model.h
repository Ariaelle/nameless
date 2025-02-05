#pragma once
#include "nameless_device.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace nameless {
	class NamelessModel {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};
		NamelessModel(NamelessDevice &device, const std::vector<Vertex> &vertices);
		~NamelessModel();
		

		//Copy is not allowed
		NamelessModel(const NamelessModel&) = delete;
		NamelessModel& operator=(const NamelessModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private: 

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		NamelessDevice& namelessDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}
