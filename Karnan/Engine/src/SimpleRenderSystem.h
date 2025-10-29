#pragma once
#include "KarnanDevice.h"
#include "KarnanPipeline.h"

#include "BasicMesh.h"

#include <memory>


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

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void BindPipeline(VkCommandBuffer commandBuffer);

	void RenderObjects(VkCommandBuffer commandBuffer, BasicMesh& mesh);


private:
	void CreatePipelineLayout();
	void CreatePipeline(VkRenderPass renderPass);
};