#include "EditorDetailsPanel.h"

#include "../EngineCore.h"
#include "KarnanEditor.h"


#include "glm/gtc/type_ptr.hpp"

#include "imgui_stdlib.h"
//#include "imgui_stdlib.cpp"


void EditorDetailsPanel::OnImGUIRender()
{
	if (KarnanEditor::Instance->GetCurrentSelectedItem() != _lastFrameSelected)
	{
		_selectedChanged = true;
	}


	bool detailsOpen = true;
	ImGui::Begin("Details", &detailsOpen);
	switch (KarnanEditor::Instance->GetCurrentSelectedType())
	{
	case KarnanEditor::DetailsPanelTypes::NONE:
		break;
	case KarnanEditor::DetailsPanelTypes::GAMEOBJECT:
		DisplayGameObject();
		break;
	case KarnanEditor::DetailsPanelTypes::MATERIAL:
		DisplayMaterialData();
		break;
	}
	ImGui::End();
	_lastFrameSelected = KarnanEditor::Instance->GetCurrentSelectedItem();
	if (_selectedChanged)
		_selectedChanged = false;
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

void EditorDetailsPanel::DisplayGameObject()
{
	GameObject* lastHighlightedGO = KarnanEditor::Instance->GetLastHighlightedGO();
	if (lastHighlightedGO == nullptr)
	{
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
		ImGui::BeginChild("Mesh List", ImVec2(250.0f, 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
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
		DeleteGameObject(lastHighlightedGO);
	}
}

void EditorDetailsPanel::DisplayMaterialData()
{
	MaterialDataObject* materialData = (MaterialDataObject*)KarnanEditor::Instance->GetCurrentSelectedItem();
	if (_selectedChanged)
	{
		_materialCopyObject = MaterialDataObject(*materialData);
	}

	ImGui::Text("Material Data");
	ImGui::Separator();

	ImGui::Text("Material Name: ");
	std::string GOName = _materialCopyObject.MaterialName;
	ImGui::InputText("Object Name", &GOName);
	_materialCopyObject.MaterialName = GOName;
	//ImGui::Text(_materialCopyObject.MaterialName.c_str());
	
	ImGui::Text("Textures: ");
	for (int i = 0; i < 8; i++)
	{
		ImGui::PushID(i);
		
		ImGui::Text((std::to_string(i) + ": ").c_str());
		if (ImGui::BeginPopupContextItem("ChangeTexturePopup"))
		{
			ImGui::SeparatorText("Choose new texture");
			ImGui::BeginChild("Mesh List", ImVec2(250.0f, 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
			std::vector<std::string> textureNames = AssetManager::Instance->FindTexturePathsInAssetFolder();
			for (auto textureName : textureNames)
			{
				if (ImGui::Selectable(textureName.c_str()))
				{
					_materialCopyObject.Textures[i] = textureName;
				}
			}
			ImGui::EndChild();
			ImGui::EndPopup();
		}

		std::string selectableText = "None";
		if (_materialCopyObject.Textures[i] != "")
		{
			selectableText = _materialCopyObject.Textures[i];
		}
		if (ImGui::Selectable(selectableText.c_str()))
		{
			ImGui::OpenPopup("ChangeTexturePopup");
		}

		ImGui::PopID();
	}
	ImGui::Separator();
	if (ImGui::Button("Discard Changes"))
	{
		DiscardMaterialChanges();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Changes"))
	{
		SaveMaterialChanges();
	}

}

void EditorDetailsPanel::SaveMaterialChanges()
{
	MaterialDataObject* materialData = (MaterialDataObject*)KarnanEditor::Instance->GetCurrentSelectedItem();
	materialData->UpdateData(_materialCopyObject);
}

void EditorDetailsPanel::DiscardMaterialChanges()
{
	MaterialDataObject* materialData = (MaterialDataObject*)KarnanEditor::Instance->GetCurrentSelectedItem();
	_materialCopyObject = MaterialDataObject(*materialData);
}
