#pragma once

#include "IPanel.h"
#include "../GameObject.h"
#include "../AssetManagement/MaterialDataObject.h"


class EditorDetailsPanel : public IPanel
{
public:


private:
	void* _lastFrameSelected = nullptr;
	bool _selectedChanged = false;
	
	
	MaterialDataObject _materialCopyObject = {};


public:
	virtual void OnImGUIRender() override;

	void DeleteGameObject(GameObject* go);

private:
	void ChangeMeshButton();
	void DisplayGameObject();
	void DisplayMaterialData();

	void SaveMaterialChanges();
	void DiscardMaterialChanges();

};