#include "EditorContentBrowser.h"

#include "KarnanEditor.h"

#include <iostream>
#include "../AssetManagement/MaterialDataObject.h"
#include "../AssetManagement/AssetManager.h"


void EditorContentBrowser::OnImGUIRender()
{
	bool gameObjectBuilderOpen = true;
	ImGui::Begin("Content Browser", &gameObjectBuilderOpen);
	if (ImGui::BeginPopupContextWindow())
	{
		ContextMenu();
	}


	if (ImGui::Button("<---"))
	{
		if (_workingFilepath.parent_path() != "")
		{
			_workingFilepath = _workingFilepath.parent_path();
		}
	}


	for (auto& file : std::filesystem::directory_iterator(_workingFilepath))
	{
		if (file.is_directory())
		{
			if ( ImGui::Button( file.path().string().c_str() ) )
			{
				_workingFilepath = file.path();
			}
		}
		else
		{
			if (ImGui::Selectable(file.path().string().c_str()))
			{
				if (AssetManager::Instance->IsMaterialDataLoaded(file.path().generic_string()))
				{
					KarnanEditor::Instance->SetMaterialAsSelected(file.path().generic_string());
				}
			}
		}
	}


	ImGui::End();
}

void EditorContentBrowser::ContextMenu()
{
	if (ImGui::Button("Create New Material"))
	{
		AssetManager::Instance->AddNewMaterialDataObject();
	}
	ImGui::EndPopup();
}
