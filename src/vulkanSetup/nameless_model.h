#pragma once
#include "nameless_device.h"
#include "nameless_buffer.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace nameless {
	class NamelessModel {
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
			
			void loadModel(const std::string& filepath);
		};

		NamelessModel(NamelessDevice &device, const NamelessModel::Builder &builder);
		~NamelessModel();
		

		//Copy is not allowed
		NamelessModel(const NamelessModel&) = delete;
		NamelessModel& operator=(const NamelessModel&) = delete;

		static std::unique_ptr<NamelessModel> createModelFromFile(NamelessDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private: 

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		NamelessDevice& namelessDevice;
		std::unique_ptr<NamelessBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<NamelessBuffer> indexBuffer;
		uint32_t indexCount;
	};
}
