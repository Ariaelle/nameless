#include "movement_controller.h"

namespace nameless {
	void MovementController::moveInPlaneXZ(GLFWwindow* window, float dt, NamelessGameObject& gameObject) {
		
		//May change this
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		static bool cursorTogglePressed = false;
		static double previousMouseXPos{ 0.0 };
		static double previousMouseYPos{ 0.0 };

		
		if (glfwGetKey(window, keys.cursorToggle) == GLFW_PRESS && !cursorTogglePressed) {
			if (cursorToggled) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					cursorToggled = false;
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				cursorToggled = true;
			}
			cursorTogglePressed = true;
		}
		if (glfwGetKey(window, keys.cursorToggle) == GLFW_RELEASE) {
			cursorTogglePressed = false; // Reset when key is released
		}


		if (cursorToggled) {
			
			//properly triggers input mode when initializing :)
			if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}

			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);

			float deltaX = static_cast<float>(xPos - previousMouseXPos);
			float deltaY = static_cast<float>(yPos - previousMouseYPos);

			previousMouseXPos = xPos;
			previousMouseYPos = yPos;

			float mouseDeadZone = 0.005f;
			if (glm::abs(deltaX) < mouseDeadZone) deltaX = 0.0f;
			if (glm::abs(deltaY) < mouseDeadZone) deltaY = 0.0f;


			glm::vec3 rotate{ 0 };
			rotate.x += -deltaY;
			rotate.y += deltaX;

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.rotation += mouseSensitivity * lookSpeed * dt * glm::normalize(rotate);
			}
			gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
			gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
		}
		else {

			glm::vec3 rotate{ 0 };
			if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;


			//Will prevent errors resulting from a rotation still 0. 
			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
			}

			gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
			gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
		}

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir = glm::cross(rightDir, forwardDir);

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;



		if (glfwGetKey(window, keys.speedUp) == GLFW_PRESS) {
			//Prevents errors from normalizing zero vector.
			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.translation += speedUp * moveSpeed * dt * glm::normalize(moveDir);
			}
		}
		else {
			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
			}
		}
	}
}