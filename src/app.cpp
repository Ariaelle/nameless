#include "app.h"
#include <stdexcept>
#include "nameless_camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <chrono>
#include "movement_controller.h"
#include "vulkanSetup/nameless_buffer.h"
#include <iostream>

namespace nameless {

	app::app() {
        globalPool = NamelessDescriptorPool::Builder(namelessDevice)
            .setMaxSets(NamelessSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NamelessSwapChain::MAX_FRAMES_IN_FLIGHT).build();
		loadGameObjects();
	}
	app::~app() {}
	 
    void app::run() {

        std::vector<std::unique_ptr<NamelessBuffer>> uniformBuffers(NamelessSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uniformBuffers.size(); i++) {
           uniformBuffers[i] = std::make_unique<NamelessBuffer>(
                namelessDevice,
                sizeof(GlobalUniformBufferObject),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                namelessDevice.properties.limits.minUniformBufferOffsetAlignment
            );
           uniformBuffers[i]->map();
        }
        
        auto globalSetLayout = NamelessDescriptorSetLayout::Builder(namelessDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(NamelessSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uniformBuffers[i]->descriptorInfo();
            NamelessDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
        }

		BaseRenderSystem baseRenderSystem(namelessDevice, namelessRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        PointLightSystem pointLightSystem(namelessDevice, namelessRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        NamelessCamera camera{};
        
        auto viewerObject = NamelessGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        MovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!namelessWindow.shouldClose()) {
			
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();

            auto frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(namelessWindow.getGLFWWindow(), frameTime, viewerObject);

            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = namelessRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 30.f);

			if (auto commandBuffer = namelessRenderer.beginFrame()) {
                int frameIndex = namelessRenderer.getFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};

                //update memory
                GlobalUniformBufferObject ubo{};
                ubo.projectionMatrix= camera.getProjection();
                ubo.viewMatrix = camera.getView();
                pointLightSystem.update(frameInfo, ubo);
                uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                uniformBuffers[frameIndex]->flush();

                //render
				namelessRenderer.beginSwapChainRenderPass(commandBuffer);
				baseRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
				namelessRenderer.endSwapChainRenderPass(commandBuffer);
				namelessRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(namelessDevice.device());
	}

	void app::loadGameObjects() {
        std::shared_ptr<NamelessModel> smoothVaseModel = NamelessModel::createModelFromFile(namelessDevice, "models/smooth_vase.obj");
        std::shared_ptr<NamelessModel> floorModel = NamelessModel::createModelFromFile(namelessDevice, "models/quad_model.obj");

        auto smoothVase = NamelessGameObject::createGameObject();
        smoothVase.model = smoothVaseModel;
        smoothVase.transform.translation = { .5f, .5f, 0.f };
        smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

        auto floor = NamelessGameObject::createGameObject();
        floor.model = floorModel;
        floor.transform.translation = { 0.f, 0.5f, 0.f };
        floor.transform.scale = { 3.f, 1.5, 3.f };
        gameObjects.emplace(floor.getId(), std::move(floor));

        

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}
        };
        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = NamelessGameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(glm::mat4(1.f), (i * glm::two_pi<float>() / lightColors.size()), { 0.f, -1.f, 0.f });
            pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -2.f, -1.f, 1.f));
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }
        
	}
}