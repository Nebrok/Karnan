#include "KarnanEditor.h"
#include "../EngineCore.h"

#include "EditorHierarchyPanel.h"
#include "EditorDetailsPanel.h"
#include "EditorGOCreatorPanel.h"
#include "EditorContentBrowser.h"

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

	_panels.push_back(new EditorHierarchyPanel());
	_panels.push_back(new EditorDetailsPanel());
	_panels.push_back(new EditorGOCreatorPanel());
	_panels.push_back(new EditorContentBrowser());

}

void KarnanEditor::Update()
{
	_mainGUI->NewFrame();
	for (auto panel : _panels)
	{
		panel->OnImGUIRender();
	}
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
	for (auto panel : KarnanEditor::Instance->_panels)
	{
		delete(panel);
	}

	KarnanMainGUI* gui = KarnanEditor::Instance->_mainGUI.release();
	delete(gui);

	delete(KarnanEditor::Instance);
}
