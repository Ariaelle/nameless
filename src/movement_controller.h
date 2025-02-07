#pragma once

#include "nameless_game_object.h"
#include "vulkanSetup/nameless_window.h"

namespace nameless {
	class MovementController {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_SPACE;
			int moveDown = GLFW_KEY_LEFT_CONTROL;
			int lookLeft = GLFW_KEY_LEFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
			int cursorToggle = GLFW_KEY_LEFT_ALT;
			int speedUp = GLFW_KEY_LEFT_SHIFT;
		};

		void moveInPlaneXZ(GLFWwindow* window, float dt, NamelessGameObject& gameObject);

		KeyMappings keys{};
		float moveSpeed{ 3.f };
		float lookSpeed{ 1.5f };
		bool cursorToggled = false;
		float mouseSensitivity{ 3.f };
		float speedUp{ 1.5f };
	};
}