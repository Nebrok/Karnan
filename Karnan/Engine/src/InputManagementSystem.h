#pragma once
#include "KarnanWindow.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <unordered_map>

class InputManagementSystem
{

public:
	static InputManagementSystem* Instance;

	enum class SupportedKeys
	{
		w = GLFW_KEY_W,
		a = GLFW_KEY_A,
		s = GLFW_KEY_S,
		d = GLFW_KEY_D,
		q = GLFW_KEY_Q,
		e = GLFW_KEY_E,
		space = GLFW_KEY_SPACE,
		arrow_up = GLFW_KEY_UP,
		arrow_left = GLFW_KEY_LEFT,
		arrow_down = GLFW_KEY_DOWN,
		arrow_right = GLFW_KEY_RIGHT,
	};


private:
	std::unordered_map<InputManagementSystem::SupportedKeys, bool> _keyReadingsPressed;

	glm::vec3 _mouseWheelOffsets{ 0.0f, 0.0f, 0.0f };

public:
	static InputManagementSystem* StartupInputManagementSystem();
	static void InputSystemScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void UpdateKeyReads(GLFWwindow* window);
	bool GetKeyPressed(SupportedKeys key);
	const glm::vec3 GetScrollDelta() const { return _mouseWheelOffsets; };

private:
	InputManagementSystem();

};