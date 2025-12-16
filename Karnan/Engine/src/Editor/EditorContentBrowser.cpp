#include "EditorContentBrowser.h"

#include <iostream>


void EditorContentBrowser::OnImGUIRender()
{
	bool gameObjectBuilderOpen = true;
	ImGui::Begin("Content Browser", &gameObjectBuilderOpen);

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
			ImGui::Text(file.path().string().c_str());
		}
	}


	ImGui::End();
}
