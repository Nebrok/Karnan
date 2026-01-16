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

void KarnanRenderer::BeginGeometryRenderPass(VkCommandBuffer commandBuffer)
{
	assert(_isFrameStarted && "Cannot call BeginSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "BeginSwapChainRenderPass: provided commandBuffer does not match current");

	VkExtent2D swapchainExtent = _karnanSwapChain->GetSwapChainExtent();

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _karnanSwapChain->GetGeometryRenderPass();
	renderPassInfo.framebuffer = _karnanSwapChain->GetGeometryFrameBuffer(_currentImageIndex);

	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = swapchainExtent;

	std::array<VkClearValue, 4> clearValues{};
	clearValues[0].color = { 0.f, 0.f, 0.f, 1.0f };
	clearValues[1].color = { 0.f, 0.f, 0.f, 1.0f };
	clearValues[2].color = { 0.08f, 0.08f, 0.08f, 1.0f };
	clearValues[3].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0,0}, swapchainExtent };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void KarnanRenderer::EndGeometryRenderPass(VkCommandBuffer commandBuffer)
{
	assert(_isFrameStarted && "Cannot call EndSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "EndSwapChainRenderPass: provided commandBuffer does not match current");

	vkCmdEndRenderPass(commandBuffer);
}

void KarnanRenderer::ConfigureBarriers(VkCommandBuffer commandBuffer)
{
	/* In the end I don't think this is necessary because of the subpass dependencies
	VkImageMemoryBarrier positionImageBarrier{};
	positionImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	positionImageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	positionImageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	positionImageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	positionImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	positionImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	positionImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	positionImageBarrier.image = _karnanSwapChain->GetPositionImage();
	positionImageBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	VkImageMemoryBarrier normalImageBarrier{};
	normalImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	normalImageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	normalImageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	normalImageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	normalImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	normalImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	normalImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	normalImageBarrier.image = _karnanSwapChain->GetNormalImage();
	normalImageBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	VkImageMemoryBarrier albedoImageBarrier{};
	albedoImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	albedoImageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	albedoImageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	albedoImageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	albedoImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	albedoImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	albedoImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	albedoImageBarrier.image = _karnanSwapChain->GetAlbedoImage();
	albedoImageBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &positionImageBarrier);

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &normalImageBarrier);

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &albedoImageBarrier);
	*/

}

void KarnanRenderer::BeginLightingRenderPass(VkCommandBuffer commandBuffer)
{
	assert(_isFrameStarted && "Cannot call BeginSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "BeginSwapChainRenderPass: provided commandBuffer does not match current");

	VkExtent2D swapchainExtent = _karnanSwapChain->GetSwapChainExtent();

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _karnanSwapChain->GetLightingRenderPass();
	renderPassInfo.framebuffer = _karnanSwapChain->GetLightingFrameBuffer(_currentImageIndex);

	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = swapchainExtent;

	std::array<VkClearValue, 1> clearValues{};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0,0}, swapchainExtent };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void KarnanRenderer::EndLightingRenderPass(VkCommandBuffer commandBuffer)
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
