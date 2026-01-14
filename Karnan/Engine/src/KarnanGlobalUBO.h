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
	struct GlobalUBO
	{
		glm::mat4 projectionView{ 1.f };
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