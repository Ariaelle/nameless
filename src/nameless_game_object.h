#pragma once

#include "vulkanSetup\nameless_model.h"
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace nameless {
	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation;

		//Matrix corresponds to Translate * Ry * Rx * Rz * Scale
		//Rotations correspond to Tait-bryan YXZ
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		//Intrinsic rotation.
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	//TODO: Replace with Entity Component System
	class NamelessGameObject {

	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, NamelessGameObject>;

		static NamelessGameObject createGameObject() {
			static id_t currentId = 0;
			return NamelessGameObject(currentId++);
		}

		NamelessGameObject(const NamelessGameObject&) = delete;
		NamelessGameObject& operator=(const NamelessGameObject&) = delete;
		NamelessGameObject(NamelessGameObject&&) = default;
		NamelessGameObject& operator=(NamelessGameObject&&) = default;

		const id_t getId() { return id; }

		static NamelessGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		glm::vec3 color{};
		TransformComponent transform{};

		//Optional
		std::shared_ptr<NamelessModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		NamelessGameObject(id_t objId) : id{ objId } {}


		id_t id;
	};
}