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
	virtual void ClearInternals() override;

private:
	void AddComponentButton();
	void ChangeMeshButton();
	void ChangeMaterialButton();
	
	void DisplayGameObject();
	void DisplayMaterialData();
	void DisplayLights();
	void DisplayCamera();

	void SaveMaterialChanges();
	void DiscardMaterialChanges();


};