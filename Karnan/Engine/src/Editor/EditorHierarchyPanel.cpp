#include "EditorHierarchyPanel.h"

#include "../EngineCore.h"
#include "../GameObject.h"


void EditorHierarchyPanel::OnImGUIRender()
{
	bool hierarchyOpen = true;
	ImGui::Begin("Hierarchy", &hierarchyOpen);
	for (GameObject* go : EngineCore::GetAllGameObjectsInActiveScene())
	{
		ImGui::PushID(go->GetId());
		if (ImGui::Button(go->ObjectName.c_str()))
		{
			KarnanEditor::Instance->SetLastHighlightedGO(go);
		}
		ImGui::PopID();
	}

	ImGui::End();
}
