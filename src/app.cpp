#include "app.h"
#include <stdexcept>
#include "nameless_camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <chrono>
#include "movement_controller.h"

namespace nameless {
	app::app() {
		loadGameObjects();
	}
	app::~app() {}
	 
	void app::run() {
		BaseRenderSystem baseRenderSystem(namelessDevice, namelessRenderer.getSwapChainRenderPass());
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
				namelessRenderer.beginSwapChainRenderPass(commandBuffer);
				baseRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				namelessRenderer.endSwapChainRenderPass(commandBuffer);
				namelessRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(namelessDevice.device());
	}

	void app::loadGameObjects() {
        std::shared_ptr<NamelessModel> smoothVaseModel = NamelessModel::createModelFromFile(namelessDevice, "models/smooth_vase.obj");
        std::shared_ptr<NamelessModel> flatVaseModel = NamelessModel::createModelFromFile(namelessDevice, "models/flat_vase.obj");
        std::shared_ptr<NamelessModel> cubeModel = NamelessModel::createModelFromFile(namelessDevice, "models/cube.obj");
        std::shared_ptr<NamelessModel> coloredCubeModel = NamelessModel::createModelFromFile(namelessDevice, "models/colored_cube.obj");
        auto cube = NamelessGameObject::createGameObject();
        cube.model = cubeModel;
        cube.transform.translation = { 0.f, 0.f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        gameObjects.push_back(std::move(cube));

        auto coloredCube = NamelessGameObject::createGameObject();
        coloredCube.model = coloredCubeModel;
        coloredCube.transform.translation = { 2.75f, 1.35f, 4.5f };
        coloredCube.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(coloredCube));

        auto smoothVase = NamelessGameObject::createGameObject();
        smoothVase.model = smoothVaseModel;
        smoothVase.transform.translation = { 1.5f, 2.35f, 4.5f };
        smoothVase.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(smoothVase));

        auto flatVase = NamelessGameObject::createGameObject();
        flatVase.model = flatVaseModel;
        flatVase.transform.translation = { 2.5f, 4.35f, 8.5f };
        flatVase.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(flatVase));
	}
}