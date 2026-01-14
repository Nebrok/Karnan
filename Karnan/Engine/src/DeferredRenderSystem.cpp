#include "DeferredRenderSystem.h"

void DeferredRenderSystem::CreateUniformBuffers()
{
	_globalUBOBuffers.resize(_maxFramesInFlight);
	for (int i = 0; i < _globalUBOBuffers.size(); i++)
	{
		_globalUBOBuffers[i] = std::make_unique<KarnanGlobalUBO>(_karnanDevice);
	}
}

void DeferredRenderSystem::SetupDescriptors()
{
	_globalPool = KarnanDescriptorPool::Builder(_karnanDevice)
		.setMaxSets(1000)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _maxFramesInFlight)
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
		.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();
	_lightingDescriptorSetLayout = move(lightingSetLayout);


}

void DeferredRenderSystem::SetupPipelines()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);



}
