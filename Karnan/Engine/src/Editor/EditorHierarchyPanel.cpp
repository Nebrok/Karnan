#include "EditorHierarchyPanel.h"

#include "../EngineCore.h"
#include "../GameObject.h"
#include "../Lights/PointLight.h"
#include "../SpecialGameObjects/TerrainObject.h"
#include "../SpecialGameObjects/PlayerObject.h"
#include "../SpecialGameObjects/Stopwatch.h"


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
			else if (go->HasTag("Camera"))
			{
				KarnanEditor::Instance->SetCameraAsSelected((KarnanCamera*)go);
			}
			else if (go->HasTag("Terrain"))
			{
				KarnanEditor::Instance->SetTerrainObjectAsSelected((TerrainObject*)go);
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
		newGo->Init();
		newGo->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newGo->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<GameObject>(newGo));
	}
	if (ImGui::Button("Create New PointLight"))
	{
		PointLight* newLight = DBG_NEW PointLight();
		newLight->Init();
		newLight->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newLight->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		newLight->SetColour({ 1.0f, 1.0f, 1.0f });
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<PointLight>(newLight));
	}
	if (ImGui::Button("Create New TerrainObject"))
	{
		TerrainObject* newTerrain = DBG_NEW TerrainObject();
		newTerrain->Init();
		newTerrain->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newTerrain->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<TerrainObject>(newTerrain));
	}
	if (ImGui::Button("Create New PlayerObject"))
	{
		PlayerObject* newPlayer = DBG_NEW PlayerObject();
		newPlayer->Init();
		newPlayer->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newPlayer->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<PlayerObject>(newPlayer));
	}
	if (ImGui::Button("Create New Stopwatch"))
	{
		Stopwatch* newStopwatch = DBG_NEW Stopwatch();
		newStopwatch->Init();
		newStopwatch->Transform.Translation = { 0.0f, 0.0f, 0.0f };
		newStopwatch->Transform.Scale = { 1.0f, 1.0f, 1.0f };
		EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<Stopwatch>(newStopwatch));
	}
	ImGui::EndPopup();
}
