#include "EditorHierarchyPanel.h"

#include "../EngineCore.h"
#include "../GameObject.h"


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
			KarnanEditor::Instance->SetLastHighlightedGO(sharedGo);
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
	ImGui::EndPopup();
}
