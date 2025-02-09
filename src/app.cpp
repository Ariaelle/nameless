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

namespace nameless {

    struct GlobalUniformBufferObject {
        alignas(16) glm::mat4 projectionView{ 1.f };
        //alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, 3.f, -1.f });
        
        alignas(16) glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.02f };
        alignas(16) glm::vec3 lightPosition{ -1.f };
        alignas(16) glm::vec4 lightColor{ 1.f, 1.f, 1.f, 1.f };
    };


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
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(NamelessSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uniformBuffers[i]->descriptorInfo();
            NamelessDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
        }

		BaseRenderSystem baseRenderSystem(namelessDevice, namelessRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        NamelessCamera camera{};
        
        auto viewerObject = NamelessGameObject::createGameObject();
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
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

                //update memory
                GlobalUniformBufferObject ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uniformBuffers[frameIndex]->writeToBuffer(&ubo);
                uniformBuffers[frameIndex]->flush();

                //render
				namelessRenderer.beginSwapChainRenderPass(commandBuffer);
				baseRenderSystem.renderGameObjects(frameInfo, gameObjects);
				namelessRenderer.endSwapChainRenderPass(commandBuffer);
				namelessRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(namelessDevice.device());
	}

	void app::loadGameObjects() {
        std::shared_ptr<NamelessModel> smoothVaseModel = NamelessModel::createModelFromFile(namelessDevice, "models/smooth_vase.obj");
      //  std::shared_ptr<NamelessModel> flatVaseModel = NamelessModel::createModelFromFile(namelessDevice, "models/flat_vase.obj");
        //std::shared_ptr<NamelessModel> cubeModel = NamelessModel::createModelFromFile(namelessDevice, "models/cube.obj");
       // std::shared_ptr<NamelessModel> coloredCubeModel = NamelessModel::createModelFromFile(namelessDevice, "models/colored_cube.obj");
       /* auto cube = NamelessGameObject::createGameObject();
        cube.model = cubeModel;
        cube.transform.translation = { 0.f, 0.f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        gameObjects.push_back(std::move(cube));
        */
       /* auto coloredCube = NamelessGameObject::createGameObject();
        coloredCube.model = coloredCubeModel;
        coloredCube.transform.translation = { 2.75f, 1.35f, 4.5f };
        coloredCube.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(coloredCube));
        */

        auto smoothVase = NamelessGameObject::createGameObject();
        smoothVase.model = smoothVaseModel;
        smoothVase.transform.translation = { .5f, .5f, 0.f };
        smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
        gameObjects.push_back(std::move(smoothVase));

    //    auto flatVase = NamelessGameObject::createGameObject();
    //    flatVase.model = flatVaseModel;
    //    flatVase.transform.translation = { 2.5f, 4.35f, 8.5f };
    //    flatVase.transform.scale = { 1.0f, 1.0f, 1.0f };
    //    gameObjects.push_back(std::move(flatVase));
        
	}
}