#pragma once

#include "vulkanSetup\nameless_model.h"
#include <memory>


namespace nameless {

	//TODO: Replace with Entity Component System
	class NamelessGameObject {

		struct Transform2dComponent {
			glm::vec2 translation{};
			glm::vec2 scale{1.f, 1.f};
			float rotation;

			glm::mat2 mat2() {
				const float s = glm::sin(rotation);
				const float c = glm::cos(rotation);
				glm::mat2 rotMatrix{ {c, s}, {-s, c}};

				glm::mat2 scaleMat{ {scale.x, 0.f }, {0.f, scale.y} };

				return rotMatrix * scaleMat;
			}
		};

	public:
		using id_t = unsigned int;

		static NamelessGameObject createGameObject() {
			static id_t currentId = 0;
			return NamelessGameObject(currentId++);
		}

		NamelessGameObject(const NamelessGameObject&) = delete;
		NamelessGameObject& operator=(const NamelessGameObject&) = delete;
		NamelessGameObject(NamelessGameObject&&) = default;
		NamelessGameObject& operator=(NamelessGameObject&&) = default;

		const id_t getId() { return id; }

		std::shared_ptr<NamelessModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d;


	private:
		NamelessGameObject(id_t objId) : id{ objId } {}


		id_t id;
	};
}