#pragma once

#include "IPanel.h"

#include <filesystem>

class EditorContentBrowser : public IPanel
{

public:


private:

	std::filesystem::path _workingFilepath = { "assets" };

public:
	virtual void OnImGUIRender() override;


private:


};