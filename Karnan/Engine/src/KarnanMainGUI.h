#pragma once
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

class KarnanDevice;
class KarnanSwapChain;
class KarnanRenderer;

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


public:
	KarnanMainGUI(KarnanDevice& device, KarnanRenderer& renderer, GLFWwindow* glfwWindow);
	~KarnanMainGUI();

	void Init();
	void NewFrame();
	void Render(VkCommandBuffer& commandBuffer);


private:

};