#include "EditorHierarchyPanel.h"

#include "../EngineCore.h"
#include "../GameObject.h"


void EditorHierarchyPanel::OnImGUIRender()
{
	bool hierarchyOpen = true;
	ImGui::Begin("Hierarchy", &hierarchyOpen);
	for (std::shared_ptr<GameObject> sharedGo : EngineCore::GetAllGameObjectsInActiveScene())
	{
		GameObject* go = sharedGo.get();
		ImGui::PushID(go->GetId());
		if (ImGui::Button(go->ObjectName.c_str()))
		{
			KarnanEditor::Instance->SetLastHighlightedGO(sharedGo);
		}
		ImGui::PopID();
	}

	ImGui::End();
}
