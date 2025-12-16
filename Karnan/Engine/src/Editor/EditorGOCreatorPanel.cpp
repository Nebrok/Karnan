#include "EditorGOCreatorPanel.h"

#include "../GameObject.h"
#include "../EngineCore.h"

#include "glm/gtc/type_ptr.hpp"

void EditorGOCreatorPanel::OnImGUIRender()
{
	NewGameObjectData data{};

	bool gameObjectBuilderOpen = true;
	ImGui::Begin("Create Objects", &gameObjectBuilderOpen);

	ImGui::SeparatorText("Game Object Name");
	ImGui::Text("Eventually this will be an entry field");
	data.ObjectName = " New GameObject - Testing";
	ImGui::SeparatorText("Transform");

	ImGui::DragFloat3("Translation:", glm::value_ptr(data.Translation), 0.01f);
	ImGui::DragFloat3("Rotation:", glm::value_ptr(data.Rotation), 0.01f);
	ImGui::DragFloat3("Scale:", glm::value_ptr(data.Scale), 0.01f);

	if (ImGui::Button("Test Button"))
	{
		CreateNewGameObject(data);
	}

	ImGui::End();
}

void EditorGOCreatorPanel::CreateNewGameObject(NewGameObjectData data)
{
	GameObject* newGo = new GameObject("NewGameObject");
	newGo->CreateMesh("assets/models/SponzaSceneMaybe.obj");
	newGo->CreateMaterial("textures/Staff_low_lambert1_BaseColor.png");
	newGo->Transform.Scale = { 1.0f, 1.0f, 1.0f };
	EngineCore::Instance->AddGameObjectToActiveScene(std::shared_ptr<GameObject>(newGo));
}