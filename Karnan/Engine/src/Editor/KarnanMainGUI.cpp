#include "KarnanMainGUI.h"
#include "imgui_internal.h"

#include <stdexcept>

#include "../EngineCore.h"
#include "../KarnanDevice.h"
#include "../KarnanRenderer.h"
#include "../KarnanSwapChain.h"

#include "glm/gtc/type_ptr.hpp"

static void check_vk_result(VkResult err)
{
	if (err == VK_SUCCESS)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

KarnanMainGUI::KarnanMainGUI(KarnanDevice& device, KarnanRenderer& renderer, GLFWwindow* glfwWindow)
	: _karnanDevice(device), _karnanRenderer(renderer), _windowRef(glfwWindow)
{

}

void KarnanMainGUI::Init()
{
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& IO = ImGui::GetIO(); (void)IO;
	GuiIO = &IO;
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	IO.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	IO.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Create Descriptor Pool
	// If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 0;
		for (VkDescriptorPoolSize& pool_size : pool_sizes)
			pool_info.maxSets += pool_size.descriptorCount;
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		if (vkCreateDescriptorPool(_karnanDevice.Device(), &pool_info, nullptr, &_guiDescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("IMGUI descriptor pool init error");
		}
	}

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(_windowRef, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
	init_info.Instance = _karnanDevice.GetVkInstance();
	init_info.PhysicalDevice = _karnanDevice.GetVkPhysicalInstance();
	init_info.Device = _karnanDevice.Device();
	init_info.QueueFamily = _karnanDevice.GraphicsQueueFamily();
	init_info.Queue = _karnanDevice.GraphicsQueue();
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = _guiDescriptorPool;
	init_info.MinImageCount = KarnanSwapChain::MAX_FRAMES_IN_FLIGHT;
	init_info.ImageCount = KarnanSwapChain::MAX_FRAMES_IN_FLIGHT;
	init_info.Allocator = nullptr;
	init_info.PipelineInfoMain.RenderPass = _karnanRenderer.GetSwapChainLightingRenderPass();
	init_info.PipelineInfoMain.Subpass = 0;
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info);
	/*ImGUI end*/
}

void KarnanMainGUI::NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoSavedSettings;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_MenuBar;

	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
	dockspaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;


	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	bool windowOpen = true;
	ImGui::Begin("Main Viewport", NULL, window_flags);

	if (ImGui::BeginMenuBar())
	{
		ImGui::Text("Scene");

		ImGui::SameLine();
		
		float titleBarHeight = ImGui::GetFrameHeight();
		float buttonSize = titleBarHeight - 2.0f;

		if (ImGui::SmallButton("Play"))
		{
			if (EngineCore::Instance->PlayMode() == false && KarnanEditor::Instance->Paused() == true)
			{
				EngineCore::Instance->SetPlayMode(true);
				KarnanEditor::Instance->SetPaused(false);

			}
			else if (EngineCore::Instance->PlayMode() == false)
			{
				KarnanEditor::Instance->CacheScene();
				EngineCore::Instance->SetPlayMode(true);
				KarnanEditor::Instance->SetPaused(false);
			}
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Pause"))
		{
			EngineCore::Instance->SetPlayMode(false);
			KarnanEditor::Instance->SetPaused(true);
		}

		ImGui::SameLine();
		if (ImGui::SmallButton("Stop"))
		{
			EngineCore::Instance->SetPlayMode(false);
			KarnanEditor::Instance->LoadCachedScene();
		}

		ImGui::EndMenuBar();
	}
	
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
	}



	ImGui::End();
	

	bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
}

void KarnanMainGUI::EndFrame()
{
	ImGui::Render();

	// Update and Render additional Platform Windows
	if (GuiIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void KarnanMainGUI::Render(VkCommandBuffer& commandBuffer)
{
	ImDrawData* main_draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(main_draw_data, commandBuffer);

}

KarnanMainGUI::~KarnanMainGUI()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(_karnanDevice.Device(), _guiDescriptorPool, nullptr);
}
