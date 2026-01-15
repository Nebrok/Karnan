#pragma once

//std libs
#include <vector>
#include <string>

#include "KarnanDevice.h"


struct PipelineConfigInfo
{
	PipelineConfigInfo() {};

	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment1;
	VkPipelineColorBlendAttachmentState colorBlendAttachment2;
	VkPipelineColorBlendAttachmentState colorBlendAttachment3;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class KarnanPipeline
{
public:


private:
	KarnanDevice& _karnanDevice;
	VkPipeline _graphicsPipeline;
	VkShaderModule _vertShaderModule;
	VkShaderModule _fragShaderModule;

public:
	KarnanPipeline(KarnanDevice& device, const PipelineConfigInfo& configInfo, const std::string& vertFilePath,	const std::string& fragFilePath);
	~KarnanPipeline();

	KarnanPipeline(const KarnanPipeline&) = delete;
	KarnanPipeline& operator=(const KarnanPipeline&) = delete;

	void Bind(VkCommandBuffer commandBuffer);

	static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	static void GeometryPipelineConfigInfo(PipelineConfigInfo& configInfo);
	static void LightingPipelineConfigInfo(PipelineConfigInfo& configInfo);

private:
	static std::vector<char> ReadFile(const std::string& filepath);

	void CreateGraphicsPipeline(const PipelineConfigInfo& configInfo, const std::string& vertShader, const std::string& fragShader);

	void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

};