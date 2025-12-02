#include "EditorDetailsPanel.h"

#include "../EngineCore.h"
#include "KarnanEditor.h"
#include "../GameObject.h"

#include "glm/gtc/type_ptr.hpp"

void EditorDetailsPanel::OnImGUIRender()
{
	bool detailsOpen = true;
	ImGui::Begin("Details", &detailsOpen);

	GameObject* lastHighlightedGO = KarnanEditor::Instance->GetLastHighlightedGO();
	if (lastHighlightedGO == nullptr)
	{
		ImGui::End();
		return;
	}

	ImGui::SeparatorText("Game Object Name");
	ImGui::Text(lastHighlightedGO->ObjectName);
	ImGui::SeparatorText("Transform");

	ImGui::DragFloat3("Translation:", glm::value_ptr(lastHighlightedGO->Transform.Translation), 0.01f);
	ImGui::DragFloat3("Rotation:", glm::value_ptr(lastHighlightedGO->Transform.Rotation), 0.01f);
	ImGui::DragFloat3("Scale:", glm::value_ptr(lastHighlightedGO->Transform.Scale), 0.01f);

	ImGui::End();
}
