#include "DeferredRenderSystem.h"

#include <stdexcept>

#include "Lights/PointLight.h"
#include "VertexBuffer.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

DeferredRenderSystem::DeferredRenderSystem(KarnanDevice& device, KarnanSwapChain* swapChain, int maxFramesInFlight)
	: _karnanDevice(device), _swapChain(swapChain), _maxFramesInFlight(maxFramesInFlight)
{
	CreateUniformBuffers();
	SetupDescriptors();
	SetupPipelines();
}

DeferredRenderSystem::~DeferredRenderSystem()
{
	vkDestroyPipelineLayout(_karnanDevice.Device(), _geometryLayout, nullptr);
	vkDestroyPipelineLayout(_karnanDevice.Device(), _lightingLayout, nullptr);
}

void DeferredRenderSystem::CreateUniformBuffers()
{
	_globalUBOBuffers.resize(_maxFramesInFlight);
	for (int i = 0; i < _globalUBOBuffers.size(); i++)
	{
		_globalUBOBuffers[i] = std::make_unique<KarnanGlobalUBO>(_karnanDevice);
	}

	_lightsUBOBuffers.resize(_maxFramesInFlight);
	for (int i = 0; i < _lightsUBOBuffers.size(); i++)
	{
		_lightsUBOBuffers[i] = std::make_unique<KarnanLightsUBO>(_karnanDevice);
	}
}

void DeferredRenderSystem::SetupDescriptors()
{
	_globalPool = KarnanDescriptorPool::Builder(_karnanDevice)
		.setMaxSets(1000)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _maxFramesInFlight * 2)
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 500)
		.build();

	auto globalSetLayout = KarnanDescriptorSetLayout::Builder(_karnanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL)
		.build();
	_globalDescriptorSetLayout = move(globalSetLayout);

	_globalDescriptorSets.resize(_maxFramesInFlight);
	for (int i = 0; i < _globalDescriptorSets.size(); i++)
	{
		auto bufferInfo = _globalUBOBuffers[i]->GetDescriptorInfo();
		KarnanDescriptorWriter(*_globalDescriptorSetLayout, *_globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(_globalDescriptorSets[i]);
	}

	//The material class stores its own Descriptor set so during the render process
	//the material of the current GO is queried for the descriptor set
	auto materialSetLayout = KarnanDescriptorSetLayout::Builder(_karnanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	_materialDescriptorSetLayout = move(materialSetLayout);

	auto lightingSetLayout = KarnanDescriptorSetLayout::Builder(_karnanDevice)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL)
		.build();
	_lightsDescriptorSetLayout = move(lightingSetLayout);
	
	_lightsUboDescriptorSets.resize(_maxFramesInFlight);
	for (int i = 0; i < _lightsUboDescriptorSets.size(); i++)
	{
		auto bufferInfo = _lightsUBOBuffers[i]->GetDescriptorInfo();
		KarnanDescriptorWriter(*_lightsDescriptorSetLayout, *_globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(_lightsUboDescriptorSets[i]);
	}

}

void DeferredRenderSystem::SetupPipelines()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(DeferredPushConstantData);

	std::vector<VkDescriptorSetLayout> geometryPassDescriptorLayouts
	{ 
		_globalDescriptorSetLayout->getDescriptorSetLayout(),
		_materialDescriptorSetLayout->getDescriptorSetLayout()
	};

	VkPipelineLayoutCreateInfo geometryPipelineLayoutInfo{};
	geometryPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	geometryPipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(geometryPassDescriptorLayouts.size());
	geometryPipelineLayoutInfo.pSetLayouts = geometryPassDescriptorLayouts.data();
	geometryPipelineLayoutInfo.pushConstantRangeCount = 1;
	geometryPipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(_karnanDevice.Device(), &geometryPipelineLayoutInfo, nullptr, &_geometryLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	std::vector<VkDescriptorSetLayout> lightingPassDescriptorLayouts
	{
		_lightsDescriptorSetLayout->getDescriptorSetLayout(),
		_swapChain->GetGBufferDescriptorSetLayout()
	};

	VkPipelineLayoutCreateInfo lightingPipelineLayoutInfo{};
	lightingPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	lightingPipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(lightingPassDescriptorLayouts.size());
	lightingPipelineLayoutInfo.pSetLayouts = lightingPassDescriptorLayouts.data();
	lightingPipelineLayoutInfo.pushConstantRangeCount = 0;
	if (vkCreatePipelineLayout(_karnanDevice.Device(), &lightingPipelineLayoutInfo, nullptr, &_lightingLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}

	std::cout << "first pipeline" << '\n';


	PipelineConfigInfo geometryConfig{};
	KarnanPipeline::GeometryPipelineConfigInfo(geometryConfig);
	geometryConfig.renderPass = _swapChain->GetGeometryRenderPass();
	geometryConfig.pipelineLayout = _geometryLayout;
	_geometryPipeline = std::make_unique<KarnanPipeline>(
		_karnanDevice,
		geometryConfig,
		"shaders/deferredGeoShader.vert.spv",
		"shaders/deferredGeoShader.frag.spv");

	std::cout << "Second pipeline" << '\n';

	PipelineConfigInfo pipelineConfig{};
	KarnanPipeline::DefaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = _swapChain->GetLightingRenderPass();
	pipelineConfig.pipelineLayout = _lightingLayout;
	_lightingPipeline = std::make_unique<KarnanPipeline>(
		_karnanDevice,
		pipelineConfig,
		"shaders/deferredLightShader.vert.spv",
		"shaders/deferredLightShader.frag.spv");

}

void DeferredRenderSystem::GeometryPass(VkCommandBuffer commandBuffer, int frameIndex, KarnanCamera* camera, std::vector<GameObject*>& gameObjects)
{
	_geometryPipeline->Bind(commandBuffer);

	Karnan::GlobalUBO ubo{};
	ubo.projectionView = camera->GetProjection() * camera->GetView();
	_globalUBOBuffers[frameIndex]->UpdateUBO(&ubo);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_geometryLayout,
		0, 1,
		&_globalDescriptorSets[frameIndex],
		0, nullptr);

	for (auto go : gameObjects)
	{
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_geometryLayout,
			1, 1,
			&GetMaterialDescriptorSet(*go->GetMaterial()),
			0, nullptr);


		DeferredPushConstantData push{};
		push.modelMatrix = go->Transform.Mat4();
		push.normalMatrix = go->Transform.NormalMatrix();

		vkCmdPushConstants(
			commandBuffer,
			_geometryLayout,
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(DeferredPushConstantData),
			&push);

		go->Render(commandBuffer);
	}

}

void DeferredRenderSystem::LightingPass(VkCommandBuffer commandBuffer, int frameIndex, int imageIndex, KarnanCamera* camera, std::vector<GameObject*>& lights, KarnanSwapChain* currentSwapchain)
{
	_lightingPipeline->Bind(commandBuffer);

	Karnan::LightingUBO lightingUbo{};
	int activeLights = 0;
	for (auto go : lights)
	{
		PointLight* light = dynamic_cast<PointLight*>(go);
		lightingUbo.lightPositions[activeLights] = glm::vec4(go->Transform.Translation, 1);
		lightingUbo.lightColours[activeLights] = glm::vec4(light->GetColour(), 1);
		lightingUbo.lightAttentuations[activeLights] = glm::vec4(light->GetAttentuation(), 1);
		lightingUbo.lightIntensities[activeLights].x = light->GetIntensity();
		++activeLights;
	}

	lightingUbo.cameraPos = glm::vec4(camera->Transform.Translation, 1);
	lightingUbo.numberLights.x = activeLights;
	_lightsUBOBuffers[frameIndex]->UpdateUBO(&lightingUbo);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_lightingLayout,
		0, 1,
		&_lightsUboDescriptorSets[frameIndex],
		0, nullptr);

	vkCmdBindDescriptorSets(
		commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_lightingLayout,
		1, 1,
		&currentSwapchain->GetCurrentGBufferDescriptorSet(imageIndex),
		0, nullptr);


	DrawFullscreenQuad(commandBuffer);

}

void DeferredRenderSystem::DrawFullscreenQuad(VkCommandBuffer commandBuffer)
{

	if (_quadBuffer == nullptr)
	{
		std::vector<VertexBuffer::Vertex> vertices =
		{
			{ {-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{ { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{ {-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{ { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{ { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{ {-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};

		_quadBuffer = DBG_NEW VertexBuffer{ _karnanDevice, vertices };
	}
	_quadBuffer->Bind(commandBuffer);
	vkCmdDraw(commandBuffer, _quadBuffer->GetVertexCount(), 1, 0, 0);
}

VkDescriptorSet& DeferredRenderSystem::GetMaterialDescriptorSet(KarnanMaterial& material)
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

