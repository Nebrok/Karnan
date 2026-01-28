#include "KarnanEditor.h"
#include "../EngineCore.h"

#include "EditorHierarchyPanel.h"
#include "EditorDetailsPanel.h"
#include "EditorGOCreatorPanel.h"
#include "EditorContentBrowser.h"

#include "../Lights/PointLight.h"
#include "../KarnanCamera.h"



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
	//_panels.push_back(new EditorGOCreatorPanel());
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

GameObject* KarnanEditor::GetLastHighlightedGO()
{
	if (_currentSelectedType != DetailsPanelTypes::GAMEOBJECT)
	{
		return nullptr;
	}
	return (GameObject*)_lastSelectedItem;
}

void KarnanEditor::SetLastHighlightedGO(GameObject* go)
{
	if (go == nullptr)
	{
		_currentSelectedType = DetailsPanelTypes::NONE;
	}
	else
	{
		_currentSelectedType = DetailsPanelTypes::GAMEOBJECT;
	}
	_lastSelectedItem = go;
}

void KarnanEditor::SetPointLightAsSelected(PointLight* pointLight)
{
	if (pointLight == nullptr)
	{
		_currentSelectedType = DetailsPanelTypes::NONE;
	}
	else
	{
		_currentSelectedType = DetailsPanelTypes::POINT_LIGHT;
	}
	_lastSelectedItem = pointLight;
}

void KarnanEditor::SetMaterialAsSelected(std::string filepath)
{
	_currentSelectedType = DetailsPanelTypes::MATERIAL;
	_lastSelectedItem = AssetManager::Instance->GetMaterialData(filepath).get();
}

void KarnanEditor::SetCameraAsSelected(KarnanCamera* camera)
{
	if (camera == nullptr)
	{
		_currentSelectedType = DetailsPanelTypes::NONE;
	}
	else
	{
		_currentSelectedType = DetailsPanelTypes::CAMERA;
	}
	_lastSelectedItem = camera;
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
