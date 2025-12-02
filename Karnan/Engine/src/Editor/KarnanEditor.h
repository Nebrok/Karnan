#pragma once
#include "KarnanMainGUI.h"

#include <memory>


class KarnanEditor
{

public:
	static KarnanEditor* Instance;

private:
	std::unique_ptr<KarnanMainGUI> _mainGUI;
	GameObject* _lastHighlightedGo;

public:
	static KarnanEditor* StartupEditor();
	static void DestroyEditor();

	void Init();
	void Update();
	void Render(VkCommandBuffer commandBuffer);

	GameObject* GetLastHighlightedGO() { return _lastHighlightedGo; };


private:
	KarnanEditor();
	~KarnanEditor();

};