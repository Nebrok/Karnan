#pragma once
#include "KarnanWindow.h"


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

public:
	static InputManagementSystem* StartupInputManagementSystem();

	void UpdateKeyReads(GLFWwindow* window);
	bool GetKeyPressed(SupportedKeys key);

private:
	InputManagementSystem();

};