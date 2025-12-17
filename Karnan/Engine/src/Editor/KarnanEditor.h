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
	std::shared_ptr<GameObject> _lastHighlightedGo = nullptr;
	std::unique_ptr<KarnanMainGUI> _mainGUI;

	std::vector<IPanel*> _panels;


public:
	static KarnanEditor* StartupEditor();
	static void DestroyEditor();

	void Init();
	void Update();
	void Render(VkCommandBuffer commandBuffer);

	std::shared_ptr<GameObject> GetLastHighlightedGO() { return _lastHighlightedGo; };
	void SetLastHighlightedGO(std::shared_ptr<GameObject> go) { _lastHighlightedGo = go; };


private:
	KarnanEditor();
	~KarnanEditor();

};