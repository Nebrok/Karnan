#pragma once
#include "KarnanMainGUI.h"

#include "IPanel.h"


#include <memory>
#include <vector>

class KarnanEditor
{

public:
	static KarnanEditor* Instance;

private:
	GameObject* _lastHighlightedGo = nullptr;
	std::unique_ptr<KarnanMainGUI> _mainGUI;

	std::vector<IPanel*> _panels;


public:
	static KarnanEditor* StartupEditor();
	static void DestroyEditor();

	void Init();
	void Update();
	void Render(VkCommandBuffer commandBuffer);

	GameObject* GetLastHighlightedGO() { return _lastHighlightedGo; };
	void SetLastHighlightedGO(GameObject* go) { _lastHighlightedGo = go; };


private:
	KarnanEditor();
	~KarnanEditor();

};