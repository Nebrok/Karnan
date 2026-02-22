#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "KarnanDevice.h"
#include "KarnanSwapChain.h"
#include "KarnanPipeline.h"
#include "VulkanDescriptors.h"
#include "KarnanGlobalUBO.h"
#include "KarnanLightsUBO.h"
#include "KarnanMaterial.h"

class KarnanCamera;
class GameObject;
class VertexBuffer;


struct DeferredPushConstantData
{
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

class DeferredRenderSystem
{

public:

private:
	KarnanDevice& _karnanDevice;
	KarnanSwapChain* _swapChain;

	int _maxFramesInFlight;

	VkPipelineLayout _geometryLayout;
	std::unique_ptr<KarnanPipeline> _geometryPipeline;
	VkPipelineLayout _lightingLayout;
	std::unique_ptr<KarnanPipeline> _lightingPipeline;

	std::vector<std::unique_ptr<KarnanGlobalUBO>> _globalUBOBuffers;
	std::vector<std::unique_ptr<KarnanLightsUBO>> _lightsUBOBuffers;

	std::unique_ptr<KarnanDescriptorPool> _globalPool{};
	std::vector<VkDescriptorSet> _globalDescriptorSets;
	std::vector<VkDescriptorSet> _lightsUboDescriptorSets;


	std::unique_ptr<KarnanDescriptorSetLayout> _globalDescriptorSetLayout;
	std::unique_ptr<KarnanDescriptorSetLayout> _materialDescriptorSetLayout;
	std::unique_ptr<KarnanDescriptorSetLayout> _lightsDescriptorSetLayout;

	VertexBuffer* _quadBuffer = nullptr;

public:
	DeferredRenderSystem(KarnanDevice& device, KarnanSwapChain* swapChain, int maxFramesInFlight);
	~DeferredRenderSystem();

	DeferredRenderSystem(const DeferredRenderSystem&) = delete;
	DeferredRenderSystem& operator=(const DeferredRenderSystem&) = delete;

	KarnanDevice& GetDevice() { return _karnanDevice; }

	void CreateUniformBuffers();
	void SetupDescriptors();
	void SetupPipelines();

	void GeometryPass(VkCommandBuffer commandBuffer, int frameIndex, KarnanCamera* camera, std::vector<GameObject*>& gameObjects);
	void LightingPass(VkCommandBuffer commandBuffer, int frameIndex, int imageIndex, KarnanCamera* camera, std::vector<GameObject*>& lights, KarnanSwapChain* currentSwapchain);

private:
	void DrawFullscreenQuad(VkCommandBuffer commandBuffer);

	VkDescriptorSet& GetMaterialDescriptorSet(KarnanMaterial& material);


};