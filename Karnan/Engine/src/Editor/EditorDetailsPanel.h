#pragma once

#include "IPanel.h"
#include "../GameObject.h"

class EditorDetailsPanel : public IPanel
{
public:


private:


public:
	virtual void OnImGUIRender() override;

	void DeleteGameObject(GameObject* go);

private:


};