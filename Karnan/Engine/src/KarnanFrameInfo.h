#pragma once

#include "KarnanCamera.h"

#include "vulkan/vulkan.h"

namespace Karnan
{
	struct FrameInfo
	{
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer commandBuffer;
		float Aspect;
	};
}