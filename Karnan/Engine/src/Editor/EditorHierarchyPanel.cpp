#include "EditorHierarchyPanel.h"

#include "../EngineCore.h"
#include "../GameObject.h"
#include "../Lights/PointLight.h"


void EditorHierarchyPanel::OnImGUIRender()
{
	bool hierarchyOpen = true;
	ImGui::Begin("Hierarchy", &hierarchyOpen);
	if (ImGui::BeginPopupContextWindow())
	{
		ContextMenu();
	}


	for (std::shared_ptr<GameObject> sharedGo : EngineCore::GetAllGameObjectsInActiveScene())
	{
		GameObject* go = sharedGo.get();
		ImGui::PushID(go->GetId());
		if (ImGui::Selectable(go->ObjectName.c_str()))
		{
			if (go->HasTag("Point Light"))
			{
				KarnanEditor::Instance->SetPointLightAsSelected((PointLight*)go);
			}
			else
				KarnanEditor::Instance->SetLastHighlightedGO(go);
		}
		ImGui::PopID();
	}

	ImGui::End();
}

void EditorHierarchyPanel::ContextMenu()
{
	if (ImGui::Button("Create New GameObject"))
	{
		GameObject* newGo = DBG_NEW GameObject("NewGameObject");
		newGo->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newGo->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<GameObject>(newGo));
	}
	if (ImGui::Button("Create New PointLight"))
	{
		PointLight* newLight = DBG_NEW PointLight();
		newLight->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newLight->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		newLight->SetColour({ 1.0f, 1.0f, 1.0f });
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<PointLight>(newLight));
	}
	ImGui::EndPopup();
}
