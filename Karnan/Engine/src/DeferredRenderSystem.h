#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "KarnanDevice.h"
#include "KarnanPipeline.h"


#include "VulkanDescriptors.h"
#include "KarnanGlobalUBO.h"



struct SimplePushConstantData
{
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

class DeferredRenderSystem
{

public:

private:
	KarnanDevice& _karnanDevice;
	int _maxFramesInFlight;


	VkPipeline _geometry;
	VkPipeline _lighting;

	std::vector<std::unique_ptr<KarnanGlobalUBO>> _globalUBOBuffers;

	std::unique_ptr<KarnanDescriptorPool> _globalPool{};
	std::vector<VkDescriptorSet> _globalDescriptorSets;


	std::unique_ptr<KarnanDescriptorSetLayout> _globalDescriptorSetLayout;
	std::unique_ptr<KarnanDescriptorSetLayout> _materialDescriptorSetLayout;
	std::unique_ptr<KarnanDescriptorSetLayout> _lightingDescriptorSetLayout;



public:
	DeferredRenderSystem();
	~DeferredRenderSystem();

	DeferredRenderSystem(const DeferredRenderSystem&) = delete;
	DeferredRenderSystem& operator=(const DeferredRenderSystem&) = delete;

	KarnanDevice& GetDevice() { return _karnanDevice; }

	void CreateUniformBuffers();
	void SetupDescriptors();
	void SetupPipelines();


private:


};