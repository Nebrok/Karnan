#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>

#include "KarnanBuffer.h"

#define MAX_LIGHTS 16

namespace Karnan
{
	struct GlobalUBO
	{
		glm::mat4 projectionView{ 1.f };
		glm::vec4 cameraPos;
		glm::vec4 lightPositions[MAX_LIGHTS];
		glm::vec4 lightColours[MAX_LIGHTS];
		glm::vec4 lightAttentuations[MAX_LIGHTS];
		glm::vec4 lightIntensities[MAX_LIGHTS]; //x is intensity
		glm::vec4 numberLights; //x is numbler lights
	};
}

class KarnanGlobalUBO
{
public:


private:


public:
	KarnanGlobalUBO(KarnanDevice& device);
	~KarnanGlobalUBO();

	void UpdateUBO(void* data);
	VkDescriptorBufferInfo GetDescriptorInfo();

private:
	KarnanDevice& _karnanDevice;
	std::unique_ptr<KarnanBuffer> _buffer;

};