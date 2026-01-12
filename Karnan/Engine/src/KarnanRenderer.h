#pragma once
#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanSwapChain.h"
#include "SceneManagement/KarnanScene.h"
#include "Editor/KarnanEditor.h"

//std libs
#include <cassert>
#include <memory>


class KarnanRenderer
{


private:
	KarnanWindow& _karnanWindow;
	KarnanDevice& _karnanDevice;
	std::unique_ptr<KarnanSwapChain> _karnanSwapChain;
	std::vector<VkCommandBuffer> _commandBuffers;


	uint32_t _currentImageIndex;
	int _currentFrameIndex = 0;
	bool _isFrameStarted = false;

public:
	KarnanRenderer(KarnanWindow& window, KarnanDevice& device);
	~KarnanRenderer();

	KarnanRenderer(const KarnanRenderer&) = delete;
	KarnanRenderer& operator=(const KarnanRenderer&) = delete;

	void RenderFrame(double frameTime, KarnanScene& scene, KarnanEditor& editor);


	VkCommandBuffer BeginFrame();


	void BeginGeometryRenderPass(VkCommandBuffer commandBuffer);
	void EndGeometryRenderPass(VkCommandBuffer commandBuffer);

	void ConfigureBarriers(VkCommandBuffer commandBuffer);

	void BeginLightingRenderPass(VkCommandBuffer commandBuffer);
	void EndLightingRenderPass(VkCommandBuffer commandBuffer);

	//void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	//void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void EndFrame();



	VkRenderPass GetSwapChainGeometryRenderPass() const { return _karnanSwapChain->GetGeometryRenderPass(); };
	VkRenderPass GetSwapChainLightingRenderPass() const { return _karnanSwapChain->GetLightingRenderPass(); };

	float GetAspectRatio() const { return _karnanSwapChain->ExtentAspectRatio(); }

	bool IsFrameInProgress() const { return _isFrameStarted; };
	VkCommandBuffer GetCurrentCommandBuffer() const
	{
		assert(_isFrameStarted && "Cannot get command buffer when frame is not in progress");
		return _commandBuffers[_currentFrameIndex];
	};

	int GetFrameIndex() const
	{
		assert(_isFrameStarted && "Cannot get frame index when frame is not in progress");
		return _currentFrameIndex;
	}

private:

	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void RecreateSwapchain();


};