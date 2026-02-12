#include "EditorDetailsPanel.h"

#include "../EngineCore.h"
#include "KarnanEditor.h"
#include "../Lights/PointLight.h"

#include "../Physics/Colliders.h"

#include "glm/gtc/type_ptr.hpp"

#include "imgui_stdlib.h"
//#include "imgui_stdlib.cpp"

#include "../Scripting/ScriptComponent.h"

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
	case KarnanEditor::DetailsPanelTypes::POINT_LIGHT:
		DisplayLights();
		break;
	case KarnanEditor::DetailsPanelTypes::CAMERA:
		DisplayCamera();
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

void EditorDetailsPanel::AddComponent(GameObject* lastHighlightedGo)
{
	ScriptableComponent* component = ScriptRegister::TypeMap["RotateGameObject"]();
	lastHighlightedGo->AddComponent(component);
}

void EditorDetailsPanel::ChangeMeshButton()
{
	if (ImGui::Button("Change Mesh"))
	{
		ImGui::OpenPopup("ChangeMeshPopup");
	}
}

void EditorDetailsPanel::ChangeMaterialButton()
{
	if (ImGui::Button("Change Material"))
	{
		ImGui::OpenPopup("ChangeMaterialPopup");
	}
}

void EditorDetailsPanel::DisplayGameObject()
{
	GameObject* lastHighlightedGO = KarnanEditor::Instance->GetLastHighlightedGO();
	if (lastHighlightedGO == nullptr)
	{
		return;
	}

	ImGui::Text("Gameobject");
	ImGui::Separator();

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
	if (ImGui::BeginPopupContextItem("ChangeMaterialPopup"))
	{
		ImGui::SeparatorText("Choose new material");
		ImGui::BeginChild("Material List", ImVec2(250.0f, 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		std::vector<std::string> materialNames = AssetManager::Instance->FindMaterialPathsInAssetFolder();
		for (auto materialName : materialNames)
		{
			if (ImGui::Selectable(materialName.c_str()))
			{
				lastHighlightedGO->CreateMaterial(materialName);
			}
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}
	ChangeMaterialButton();

	ImGui::SeparatorText("Collider");
	std::shared_ptr<Collider> gameObjectCollider = lastHighlightedGO->GetCollider();
	
	bool isActive = lastHighlightedGO->IsColliderActive();
	ImGui::Checkbox("Collider Active", &isActive);
	lastHighlightedGO->ChangeColliderActive(isActive);

	const char* colliderTypes[] = { "Box", "Sphere" };
	int numColliders = 2;

	if (ImGui::BeginCombo("Collider Type", gameObjectCollider->ToString().c_str()))
	{
		for (int i = 0; i < numColliders; i++)
		{
			if (ImGui::Selectable(colliderTypes[i]))
			{
				std::shared_ptr<Collider> newCollider;
				switch (i)
				{
				case 0:
					newCollider = std::shared_ptr<BoxCollider>(DBG_NEW BoxCollider());
					lastHighlightedGO->SetCollider(newCollider);
					break;
				case 1:
					newCollider = std::shared_ptr<SphereCollider>(DBG_NEW SphereCollider());
					lastHighlightedGO->SetCollider(newCollider);
					break;
				}
				newCollider->GameObject = lastHighlightedGO;
			}
		}
		ImGui::EndCombo();
	}

	if (gameObjectCollider->Type == Collider::ColliderType::BOX)
	{
		BoxCollider* box = static_cast<BoxCollider*>(gameObjectCollider.get());
		ImGui::DragFloat3("Extent", glm::value_ptr(box->Extent), 0.01f);
	}
	else if (gameObjectCollider->Type == Collider::ColliderType::SPHERE)
	{
		SphereCollider* sphere = static_cast<SphereCollider*>(gameObjectCollider.get());
		ImGui::DragFloat("Radius", &(sphere->Radius), 0.01f);
	}

	ImGui::SeparatorText("Scripts");
	auto components = lastHighlightedGO->GetComponents();
	for (auto component : components)
	{
		ImGui::Text(component->GetName().c_str());
	}
	if (ImGui::Button("Add Component"))
	{
		AddComponent(lastHighlightedGO);
	}


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

	const char* filterModes[] = { "VK_FILTER_LINEAR", "VK_FILTER_NEAREST" };
	int numFilters = 2;

	if (ImGui::BeginCombo("Min Filter Mode", MaterialDataObject::FilterToString(_materialCopyObject.MinFilter).c_str()))
	{
		for (int i = 0; i < numFilters; i++)
		{
			if (ImGui::Selectable(filterModes[i]))
				_materialCopyObject.MinFilter = MaterialDataObject::FilterNameToEnum(filterModes[i]);
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Mag Filter Mode", MaterialDataObject::FilterToString(_materialCopyObject.MagFilter).c_str()))
	{
		for (int i = 0; i < numFilters; i++)
		{
			if (ImGui::Selectable(filterModes[i]))
				_materialCopyObject.MagFilter = MaterialDataObject::FilterNameToEnum(filterModes[i]);
		}
		ImGui::EndCombo();
	}

	const char* addressModes[] = { "VK_SAMPLER_ADDRESS_MODE_REPEAT", "VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT", "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE", "VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER", "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE"};
	int numModes = 5;

	if (ImGui::BeginCombo("Sampler Address Mode", MaterialDataObject::AddressModeToString(_materialCopyObject.SamplerAddressMode).c_str()))
	{
		for (int i = 0; i < numModes; i++)
		{
			if (ImGui::Selectable(addressModes[i]))
				_materialCopyObject.SamplerAddressMode = MaterialDataObject::AddressModeNametoEnum(addressModes[i]);
		}
		ImGui::EndCombo();
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

void EditorDetailsPanel::DisplayLights()
{
	ImGui::Text("Point Light");
	ImGui::Separator();

	PointLight* lastHighlightedGO = (PointLight*)KarnanEditor::Instance->GetCurrentSelectedItem();
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

	ImGui::SeparatorText("PointLight Details");
	glm::vec3 colour = lastHighlightedGO->GetColour();
	ImGui::DragFloat3("Colour:", glm::value_ptr(colour), 0.01f);
	lastHighlightedGO->SetColour(colour);

	glm::vec3 attenuation = lastHighlightedGO->GetAttenuation();
	ImGui::DragFloat3("Attenuation:", glm::value_ptr(attenuation), 0.01f);
	lastHighlightedGO->SetAttenuation(attenuation);

	float intensity = lastHighlightedGO->GetIntensity();
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	lastHighlightedGO->SetIntensity(intensity);

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
	if (ImGui::BeginPopupContextItem("ChangeMaterialPopup"))
	{
		ImGui::SeparatorText("Choose new material");
		ImGui::BeginChild("Material List", ImVec2(250.0f, 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		std::vector<std::string> materialNames = AssetManager::Instance->FindMaterialPathsInAssetFolder();
		for (auto materialName : materialNames)
		{
			if (ImGui::Selectable(materialName.c_str()))
			{
				lastHighlightedGO->CreateMaterial(materialName);
			}
		}
		ImGui::EndChild();
		ImGui::EndPopup();
	}
	ChangeMaterialButton();

}

void EditorDetailsPanel::DisplayCamera()
{
	ImGui::Text("Camera");
	ImGui::Separator();

	KarnanCamera* highlightedCamera = (KarnanCamera*)KarnanEditor::Instance->GetCurrentSelectedItem();
	if (highlightedCamera == nullptr)
	{
		return;
	}

	ImGui::SeparatorText("Game Object Name");
	std::string GOName = highlightedCamera->ObjectName;
	ImGui::InputText("Object Name", &GOName);
	highlightedCamera->SetGOName(GOName);
	ImGui::SeparatorText("Transform");

	ImGui::DragFloat3("Translation:", glm::value_ptr(highlightedCamera->Transform.Translation), 0.01f);
	ImGui::DragFloat3("Rotation:", glm::value_ptr(highlightedCamera->Transform.Rotation), 0.01f);
	ImGui::DragFloat3("Scale:", glm::value_ptr(highlightedCamera->Transform.Scale), 0.01f);

	ImGui::SeparatorText("Camera Settings");
	float fov = highlightedCamera->GetFOV();
	ImGui::DragFloat("Vertical FOV", &fov, 1.0f);
	highlightedCamera->SetFOV(fov);

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
