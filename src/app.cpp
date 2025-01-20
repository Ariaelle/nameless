#include "app.h"
#include <stdexcept>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace nameless {

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	app::app() {
		loadGameObjects();
		createPipelineLayout();
		createPipeline();
	}
	app::~app() {
		vkDestroyPipelineLayout(namelessDevice.device(), pipelineLayout, nullptr);
	}

	void app::run() {
		while (!namelessWindow.shouldClose()) {
			glfwPollEvents();
			if (auto commandBuffer = namelessRenderer.beginFrame()) {
				namelessRenderer.beginSwapChainRenderPass(commandBuffer);
				renderGameObjects(commandBuffer);
				namelessRenderer.endSwapChainRenderPass(commandBuffer);
				namelessRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(namelessDevice.device());
	}


	void app::loadGameObjects() {
		std::vector<NamelessModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f} , {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto namelessModel = std::make_shared<NamelessModel>(namelessDevice, vertices);

		auto triangle = NamelessGameObject::createGameObject();
		triangle.model = namelessModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

	void app::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(namelessDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void app::createPipeline() {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		NamelessPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = namelessRenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		namelessPipeline = std::make_unique<NamelessPipeline>(namelessDevice, "shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv", pipelineConfig);

	}

	void app::renderGameObjects(VkCommandBuffer commandBuffer) {
		namelessPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.0001f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.transform = obj.transform2d.mat2();

			vkCmdPushConstants(commandBuffer, pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

}