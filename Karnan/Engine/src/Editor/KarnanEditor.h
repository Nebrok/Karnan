#pragma once
#include "KarnanMainGUI.h"

#include "IPanel.h"

#include <memory>
#include <vector>

class PointLight;

class KarnanEditor
{
public:
	enum class DetailsPanelTypes
	{
		NONE,
		GAMEOBJECT,
		POINT_LIGHT,
		MATERIAL
	};	

	static KarnanEditor* Instance;

private:
	std::unique_ptr<KarnanMainGUI> _mainGUI;

	std::vector<IPanel*> _panels;

	DetailsPanelTypes _currentSelectedType = DetailsPanelTypes::NONE;
	std::shared_ptr<GameObject> _lastHighlightedGo = nullptr;
	void* _lastSelectedItem = nullptr;



public:
	static KarnanEditor* StartupEditor();
	static void DestroyEditor();

	void Init();
	void Update();
	void Render(VkCommandBuffer commandBuffer);

	GameObject* GetLastHighlightedGO();
	void SetLastHighlightedGO(GameObject* go);
	void SetPointLightAsSelected(PointLight* pointLight);
	void SetMaterialAsSelected(std::string filepath);

	DetailsPanelTypes GetCurrentSelectedType() const { return _currentSelectedType; };
	void* GetCurrentSelectedItem() const { return _lastSelectedItem; };

private:
	KarnanEditor();
	~KarnanEditor();

};