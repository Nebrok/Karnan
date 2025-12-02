#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

//To get new GO Creation working
#include <string>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class KarnanDevice;
class KarnanSwapChain;
class KarnanRenderer;
class GameObject;


struct NewGameObjectData
{
	std::string ObjectName;
	glm::vec3 Translation;
	glm::vec3 Rotation;
	glm::vec3 Scale = {1.0f, 1.0f, 1.0f};
};


class KarnanMainGUI
{

public:
	ImGuiIO* GuiIO = nullptr;

private:
	KarnanDevice& _karnanDevice;
	KarnanRenderer& _karnanRenderer;
	GLFWwindow* _windowRef;

	//ImGUI descriptor Pool
	VkDescriptorPool _guiDescriptorPool;

	GameObject* _lastHighlightedGo;

public:
	KarnanMainGUI(KarnanDevice& device, KarnanRenderer& renderer, GLFWwindow* glfwWindow);
	~KarnanMainGUI();

	void Init();
	void NewFrame();
	void EndFrame();

	void Render(VkCommandBuffer& commandBuffer);

	void BuildHierarchyWindow();
	void BuildDetailsWindow();
	void BuildGameObjectCreator();
	void BuildContentBrowser();


private:

	void UpdateDetailsPanel(GameObject* go);
	void CreateNewGameObject(NewGameObjectData data);

};