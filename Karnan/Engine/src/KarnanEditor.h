#pragma once
#include "KarnanMainGUI.h"

#include <memory>


class KarnanEditor
{

public:

private:
	std::unique_ptr<KarnanMainGUI> _mainGUI;


public:
	KarnanEditor();
	~KarnanEditor();

	void Init();
	void Update();
	void Render(VkCommandBuffer commandBuffer);

	void DestroyEditor();

private:

};