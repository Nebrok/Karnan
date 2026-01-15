#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>

#include "KarnanBuffer.h"

#define FORWARD_MAX_LIGHTS 16
#define DEFERRED_MAX_LIGHTS 128

namespace Karnan
{
	struct LightingUBO
	{
		glm::vec4 cameraPos;
		glm::vec4 lightPositions[DEFERRED_MAX_LIGHTS];
		glm::vec4 lightColours[DEFERRED_MAX_LIGHTS];
		glm::vec4 lightAttentuations[DEFERRED_MAX_LIGHTS];
		glm::vec4 lightIntensities[DEFERRED_MAX_LIGHTS];
		glm::vec4 numberLights;
	};
}

class KarnanLightsUBO
{
public:


private:


public:
	KarnanLightsUBO(KarnanDevice& device);
	~KarnanLightsUBO();

	void UpdateUBO(void* data);
	VkDescriptorBufferInfo GetDescriptorInfo();

private:
	KarnanDevice& _karnanDevice;
	std::unique_ptr<KarnanBuffer> _buffer;

};