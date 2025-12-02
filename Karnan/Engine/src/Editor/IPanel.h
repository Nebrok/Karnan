#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"



class IPanel
{
public:
	virtual void OnImGUIRender() = 0;
};