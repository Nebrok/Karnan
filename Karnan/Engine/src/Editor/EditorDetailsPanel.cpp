#include "EditorDetailsPanel.h"

#include "../EngineCore.h"
#include "KarnanEditor.h"
#include "../GameObject.h"

#include "glm/gtc/type_ptr.hpp"

#include "imgui_stdlib.h"
//#include "imgui_stdlib.cpp"


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
	std::string GOName = lastHighlightedGO->ObjectName;
	ImGui::InputText("Object Name", &GOName);
	lastHighlightedGO->SetGOName(GOName);
	ImGui::SeparatorText("Transform");

	ImGui::DragFloat3("Translation:", glm::value_ptr(lastHighlightedGO->Transform.Translation), 0.01f);
	ImGui::DragFloat3("Rotation:", glm::value_ptr(lastHighlightedGO->Transform.Rotation), 0.01f);
	ImGui::DragFloat3("Scale:", glm::value_ptr(lastHighlightedGO->Transform.Scale), 0.01f);


	ImGui::SeparatorText("Material");
	//Use some sort of query to search through the assets folder and find all .kmat types
	//Then use a Gameobject set material function to pass along the filepath to the found material
	//which in turn handles the changing of the material


	ImGui::End();
}
