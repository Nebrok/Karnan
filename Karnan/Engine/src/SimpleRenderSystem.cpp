#include "SimpleRenderSystem.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


SimpleRenderSystem::SimpleRenderSystem(KarnanDevice& device, VkRenderPass renderPass, int maxFramesInFlight)
	: _karnanDevice(device), _maxFramesInFlight(maxFramesInFlight)
{
	CreateUniformBuffers();
	CreateDesciptorSets();
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

void SimpleRenderSystem::RenderObjects(Karnan::FrameInfo frameInfo, KarnanCamera& camera, std::vector<GameObject*> gameObjects)
{
	Karnan::GlobalUBO ubo{};
	ubo.projectionView = camera.GetProjection() * camera.GetView();
	_globalUBOBuffers[frameInfo.FrameIndex]->UpdateUBO(&ubo);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_pipelineLayout,
		0, 1,
		&_globalDescriptorSets[frameInfo.FrameIndex],
		0, nullptr);

	//-------------------------------

	for (auto go : gameObjects)
	{
		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_pipelineLayout,
			1, 1,
			&GetMaterialDescriptorSet(*go->GetMaterial()),
			0, nullptr);


		SimplePushConstantData push{};
		push.modelMatrix = go->Transform.Mat4();;

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			_pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push);

		go->Render(frameInfo.commandBuffer);
	}
}

void SimpleRenderSystem::CreateUniformBuffers()
{
	_globalUBOBuffers.resize(_maxFramesInFlight);
	for (int i = 0; i < _globalUBOBuffers.size(); i++)
	{
		_globalUBOBuffers[i] = std::make_unique<KarnanGlobalUBO>(_karnanDevice);
	}
}


//TODO: Fix the descriptor pool to dynamically expand to fit needs.
void SimpleRenderSystem::CreateDesciptorSets()
{
	_globalPool = KarnanDescriptorPool::Builder(_karnanDevice)
		.setMaxSets(1000)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _maxFramesInFlight)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 500)
		.build();
	
	auto globalSetLayout = KarnanDescriptorSetLayout::Builder(_karnanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.build();
	_globalSetLayout = move(globalSetLayout);

	_globalDescriptorSets.resize(_maxFramesInFlight);
	for (int i = 0; i < _globalDescriptorSets.size(); i++)
	{
		auto bufferInfo = _globalUBOBuffers[i]->GetDescriptorInfo();
		KarnanDescriptorWriter(*_globalSetLayout, *_globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(_globalDescriptorSets[i]);
	}

	//-------------------------------------------------------------------------------------------------------------------

	auto set1Layout = KarnanDescriptorSetLayout::Builder(_karnanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	_materialDescriptorSetLayout = move(set1Layout);
}

void SimpleRenderSystem::CreatePipelineLayout()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts
	{ _globalSetLayout->getDescriptorSetLayout(), 
		_materialDescriptorSetLayout->getDescriptorSetLayout()
	};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

VkDescriptorSet& SimpleRenderSystem::GetMaterialDescriptorSet(KarnanMaterial& material)
{
	if (!material.HasDescriptorSet())
	{
		KarnanDescriptorWriter writer = { *_materialDescriptorSetLayout, *_globalPool };
		for (int i = 0; i < material.GetTotalTextures(); i++)
		{
			if (material.IsTextureBoundAt(i))
				writer.writeImage(i, &(material.GetImageInfosAtIndex(i)));
		}
		writer.build(material.GetDescriptorSet());
		material.SetValidDescriptorSet();
	}
	return material.GetDescriptorSet();
}
