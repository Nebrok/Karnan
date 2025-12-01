#include "KarnanRenderer.h"

//std libs
#include <stdexcept>
#include <array>

KarnanRenderer::KarnanRenderer(KarnanWindow& window, KarnanDevice& device)
	: _karnanWindow(window), _karnanDevice(device)
{
	RecreateSwapchain();
	CreateCommandBuffers();
}

KarnanRenderer::~KarnanRenderer()
{
	FreeCommandBuffers();
}

VkCommandBuffer KarnanRenderer::BeginFrame()
{
	assert(!_isFrameStarted && "Can't call begin frame while already in progrss");

	auto result = _karnanSwapChain->AcquireNextImage(&_currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image");
	}

	_isFrameStarted = true;

	auto commandBuffer = GetCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Buffer failed to begin recording!");
	}

	return commandBuffer;
}

void KarnanRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(_isFrameStarted && "Cannot call BeginSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "BeginSwapChainRenderPass: provided commandBuffer does not match current");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _karnanSwapChain->GetRenderPass();
	renderPassInfo.framebuffer = _karnanSwapChain->GetFrameBuffer(_currentImageIndex);

	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = _karnanSwapChain->GetSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_karnanSwapChain->GetSwapChainExtent().width);
	viewport.height = static_cast<float>(_karnanSwapChain->GetSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0,0}, _karnanSwapChain->GetSwapChainExtent() };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void KarnanRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(_isFrameStarted && "Cannot call EndSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "EndSwapChainRenderPass: provided commandBuffer does not match current");

	vkCmdEndRenderPass(commandBuffer);
}

void KarnanRenderer::EndFrame()
{
	assert(_isFrameStarted && "Cannot end frame without valid frame started");
	auto commandBuffer = GetCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}

	auto result = _karnanSwapChain->SubmitCommandBuffers(&commandBuffer, &_currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _karnanWindow.WasWindowResized())
	{
		_karnanWindow.ResetWindowFlag();
		RecreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to acquire swap chain image");
	}

	_isFrameStarted = false;
	_currentFrameIndex = (_currentFrameIndex + 1) % KarnanSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void KarnanRenderer::CreateCommandBuffers()
{
	_commandBuffers.resize(KarnanSwapChain::MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = _karnanDevice.GetCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

	if (vkAllocateCommandBuffers(_karnanDevice.Device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command buffers");
	}
}

void KarnanRenderer::FreeCommandBuffers()
{
	vkFreeCommandBuffers(_karnanDevice.Device(), _karnanDevice.GetCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
	_commandBuffers.clear();
}

void KarnanRenderer::RecreateSwapchain()
{
	auto extent = _karnanWindow.GetExtent();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = _karnanWindow.GetExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(_karnanDevice.Device());


	if (_karnanSwapChain == nullptr)
	{
		_karnanSwapChain = std::make_unique<KarnanSwapChain>(_karnanDevice, extent);
	}
	else
	{
		std::shared_ptr<KarnanSwapChain> oldSwapChain = std::move(_karnanSwapChain);
		_karnanSwapChain = std::make_unique<KarnanSwapChain>(_karnanDevice, extent, oldSwapChain);

		if (!oldSwapChain->CompareSwapFormats(*_karnanSwapChain.get()))
		{
			throw std::runtime_error("Swap chain image(or depth) format has changed!");
		}

	}
}
