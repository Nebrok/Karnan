#pragma once
#include "KarnanDevice.h"
#include "KarnanPipeline.h"

#include "KarnanCamera.h"
#include "GameObject.h"

#include <memory>


struct SimplePushConstantData
{
	glm::mat4 transform{ 1.f };
	glm::mat4 modelMatrix{ 1.f };
};

class SimpleRenderSystem
{

public:

private:
	KarnanDevice& _karnanDevice;

	std::unique_ptr<KarnanPipeline> _karnanPipeline;
	VkPipelineLayout _pipelineLayout;

public:
	SimpleRenderSystem(KarnanDevice& device, VkRenderPass renderPass);
	~SimpleRenderSystem();

	KarnanDevice& GetDevice() { return _karnanDevice; }


	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void BindPipeline(VkCommandBuffer commandBuffer);

	void RenderObjects(VkCommandBuffer commandBuffer, KarnanCamera& camera, GameObject& mesh);


private:
	void CreatePipelineLayout();
	void CreatePipeline(VkRenderPass renderPass);
};