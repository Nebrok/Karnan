#pragma once
#include "KarnanDevice.h"
#include "KarnanPipeline.h"

#include "KarnanCamera.h"
#include "GameObject.h"
#include "KarnanGlobalUBO.h"
#include "KarnanFrameInfo.h"
#include "VulkanDescriptors.h"
#include "KarnanTextures.h"


#include <memory>


struct SimplePushConstantData
{
	glm::mat4 modelMatrix{ 1.f };
};

class SimpleRenderSystem
{

public:

private:
	KarnanDevice& _karnanDevice;

	std::unique_ptr<KarnanPipeline> _karnanPipeline;
	VkPipelineLayout _pipelineLayout;

	std::unique_ptr<KarnanDescriptorPool> _globalPool{};
	std::vector<std::unique_ptr<KarnanGlobalUBO>> _globalUBOBuffers;
	
	std::vector<VkDescriptorSet> _globalDescriptorSets;
	std::vector<VkDescriptorSet> _set1DescriptorSet;

	std::unique_ptr<KarnanDescriptorSetLayout> _globalSetLayout;
	std::unique_ptr<KarnanDescriptorSetLayout> _set1Layout;

	int _maxFramesInFlight;

	//temp texture
	std::unique_ptr<KarnanTexture> _defaultTexture;


public:
	SimpleRenderSystem(KarnanDevice& device, VkRenderPass renderPass, int maxFramesInFlight);
	~SimpleRenderSystem();

	KarnanDevice& GetDevice() { return _karnanDevice; }


	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void BindPipeline(VkCommandBuffer commandBuffer);

	void RenderObjects(Karnan::FrameInfo frameInfo, KarnanCamera& camera, std::vector<GameObject*> gameObjects);


private:
	void CreateUniformBuffers();
	void CreateDesciptorSets();
	void CreatePipelineLayout();
	void CreatePipeline(VkRenderPass renderPass);
};