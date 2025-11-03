#include "SimpleRenderSystem.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

SimpleRenderSystem::SimpleRenderSystem(KarnanDevice& device, VkRenderPass renderPass)
	: _karnanDevice(device)
{
	CreatePipelineLayout();
	CreatePipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem()
{
	vkDestroyPipelineLayout(_karnanDevice.Device(), _pipelineLayout, nullptr);
}

void SimpleRenderSystem::BindPipeline(VkCommandBuffer commandBuffer)
{
	_karnanPipeline->Bind(commandBuffer);
}

void SimpleRenderSystem::RenderObjects(VkCommandBuffer commandBuffer, KarnanCamera& camera, GameObject& go)
{
	
	auto projectionView = camera.GetProjection() * camera.GetView();

	SimplePushConstantData push{};
	auto modelMatrix = go.Transform.Mat4();
	push.transform = projectionView * modelMatrix;
	push.modelMatrix = modelMatrix;

	vkCmdPushConstants(
		commandBuffer,
		_pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,	
		0,
		sizeof(SimplePushConstantData),
		&push);

	go.Render(commandBuffer);
}

void SimpleRenderSystem::CreatePipelineLayout()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(_karnanDevice.Device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass)
{
	PipelineConfigInfo pipelineConfig{};
	KarnanPipeline::DefaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = _pipelineLayout;
	_karnanPipeline = std::make_unique<KarnanPipeline>(
		_karnanDevice,
		pipelineConfig,
		"shaders/simpleShader.vert.spv",
		"shaders/simpleShader.frag.spv");
}
