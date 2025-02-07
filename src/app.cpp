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

    std::unique_ptr<NamelessModel> createCubeModel(NamelessDevice& device, glm::vec3 offset) {
        std::vector<NamelessModel::Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<NamelessModel>(device, vertices);
    }

	void app::loadGameObjects() {
        std::shared_ptr<NamelessModel> namelessModel =  createCubeModel(namelessDevice, { .0f, .0f, .0f });
        
        auto cube = NamelessGameObject::createGameObject();
        cube.model = namelessModel;
        cube.transform.translation = { 0.f, 0.f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        gameObjects.push_back(std::move(cube));

        auto cube2 = NamelessGameObject::createGameObject();
        cube2.model = namelessModel;
        cube2.transform.translation = { 0.5f, 0.35f, 4.5f };
        cube2.transform.scale = { 1.0f, 1.0f, 1.0f };
        gameObjects.push_back(std::move(cube2));

	}
}