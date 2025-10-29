#pragma once
#include "KarnanWindow.h"
#include "KarnanDevice.h"
#include "KarnanSwapChain.h"

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

	VkCommandBuffer BeginFrame();
	void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void EndFrame();



	VkRenderPass GetSwapChainRenderPass() const { return _karnanSwapChain->GetRenderPass(); };
	float GetAspectRatio() const { return _karnanSwapChain->ExtentAspectRatio(); }

	bool IsFrameInProgress() const { return _isFrameStarted; };
	VkCommandBuffer GetCurrentCommandBuffer() const
	{
		assert(_isFrameStarted && "Cannot get command buffer when frame is not in progress");
		return _commandBuffers[_currentFrameIndex];
	};

private:

	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void RecreateSwapchain();


};