#include "KarnanEditor.h"
#include "../EngineCore.h"

KarnanEditor* KarnanEditor::Instance = nullptr;

KarnanEditor::KarnanEditor()
{

}

KarnanEditor::~KarnanEditor()
{

}

void KarnanEditor::Init()
{
	_mainGUI = std::make_unique<KarnanMainGUI>(EngineCore::Device(), EngineCore::Renderer(), EngineCore::Window());
	_mainGUI->Init();
}

void KarnanEditor::Update()
{
	_mainGUI->NewFrame();
	_mainGUI->BuildHierarchyWindow();
	_mainGUI->BuildDetailsWindow();
	_mainGUI->BuildGameObjectCreator();
	_mainGUI->EndFrame();
}

void KarnanEditor::Render(VkCommandBuffer commandBuffer)
{
	_mainGUI->Render(commandBuffer);
}

KarnanEditor* KarnanEditor::StartupEditor()
{
	if (KarnanEditor::Instance == nullptr)
	{
		KarnanEditor::Instance = DBG_NEW KarnanEditor();
	}

	return KarnanEditor::Instance;
}

void KarnanEditor::DestroyEditor()
{
	KarnanMainGUI* gui = KarnanEditor::Instance->_mainGUI.release();
	delete(gui);
}
