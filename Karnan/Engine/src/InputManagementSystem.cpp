#include "InputManagementSystem.h"

InputManagementSystem* InputManagementSystem::Instance = nullptr;

InputManagementSystem* InputManagementSystem::StartupInputManagementSystem()
{
	if (Instance == nullptr)
	{
		Instance = new InputManagementSystem();
	}
	return Instance;
}

void InputManagementSystem::UpdateKeyReads(GLFWwindow* window)
{
	_keyReadingsPressed[SupportedKeys::w] = glfwGetKey(window, (int)SupportedKeys::w) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::a] = glfwGetKey(window, (int)SupportedKeys::a) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::s] = glfwGetKey(window, (int)SupportedKeys::s) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::d] = glfwGetKey(window, (int)SupportedKeys::d) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::q] = glfwGetKey(window, (int)SupportedKeys::q) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::e] = glfwGetKey(window, (int)SupportedKeys::e) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::space] = glfwGetKey(window, (int)SupportedKeys::space) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::arrow_up] = glfwGetKey(window, (int)SupportedKeys::arrow_up) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::arrow_left] = glfwGetKey(window, (int)SupportedKeys::arrow_left) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::arrow_down] = glfwGetKey(window, (int)SupportedKeys::arrow_down) == GLFW_PRESS;
	_keyReadingsPressed[SupportedKeys::arrow_right] = glfwGetKey(window, (int)SupportedKeys::arrow_right) == GLFW_PRESS;
}

bool InputManagementSystem::GetKeyPressed(SupportedKeys key)
{
	return _keyReadingsPressed[key];
}

InputManagementSystem::InputManagementSystem()
{
	_keyReadingsPressed[SupportedKeys::w] = false;
	_keyReadingsPressed[SupportedKeys::a] = false;
	_keyReadingsPressed[SupportedKeys::s] = false;
	_keyReadingsPressed[SupportedKeys::d] = false;
	_keyReadingsPressed[SupportedKeys::q] = false;
	_keyReadingsPressed[SupportedKeys::e] = false;
	_keyReadingsPressed[SupportedKeys::space] = false;
	_keyReadingsPressed[SupportedKeys::arrow_up] = false;
	_keyReadingsPressed[SupportedKeys::arrow_left] = false;
	_keyReadingsPressed[SupportedKeys::arrow_down] = false;
	_keyReadingsPressed[SupportedKeys::arrow_right] = false;
}