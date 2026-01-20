#include "EditorDetailsPanel.h"

#include "../EngineCore.h"
#include "KarnanEditor.h"


#include "glm/gtc/type_ptr.hpp"

#include "imgui_stdlib.h"
//#include "imgui_stdlib.cpp"


void EditorDetailsPanel::OnImGUIRender()
{
	bool detailsOpen = true;
	ImGui::Begin("Details", &detailsOpen);

	std::shared_ptr<GameObject> lastHighlightedGO = KarnanEditor::Instance->GetLastHighlightedGO();
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

	ImGui::SeparatorText("Mesh");
	ImGui::Text("Mesh Name: ");
	ImGui::Text(lastHighlightedGO->GetMeshName().c_str());
	if (ImGui::BeginPopupContextItem("ChangeMeshPopup"))
	{
		ImGui::SeparatorText("Choose new mesh");
		ImGui::BeginChild("Mesh Lish", ImVec2(250.0f, 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		std::vector<std::string> meshNames = AssetManager::Instance->FindMeshBinariesInAssetFolder();
		for (auto meshName : meshNames)
		{
			if (ImGui::Selectable(meshName.c_str()))
			{
				lastHighlightedGO->CreateMesh(meshName);
			}
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}
	ChangeMeshButton();

	ImGui::SeparatorText("Material");
	ImGui::Text("Material Name: ");
	ImGui::Text(lastHighlightedGO->GetMaterialName().c_str());

	//Use some sort of query to search through the assets folder and find all .kmat types
	//Then use a Gameobject set material function to pass along the filepath to the found material
	//which in turn handles the changing of the material


	ImGui::SeparatorText("WARNING");
	if (ImGui::Button("Delete GameObject"))
	{
		DeleteGameObject(lastHighlightedGO.get());
	}

	ImGui::End();
}

void EditorDetailsPanel::DeleteGameObject(GameObject* go)
{
	KarnanEditor::Instance->SetLastHighlightedGO(nullptr);
	EngineCore::DeleteGOFromActiveScene(go->GetId());
}

void EditorDetailsPanel::ChangeMeshButton()
{
	if (ImGui::Button("Change Mesh"))
	{
		ImGui::OpenPopup("ChangeMeshPopup");
	}
}
