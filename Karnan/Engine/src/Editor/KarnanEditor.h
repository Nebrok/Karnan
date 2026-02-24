#pragma once
#include "KarnanMainGUI.h"

#include "IPanel.h"

#include <memory>
#include <vector>

#include "../SceneManagement/SceneDataObject.h"


class PointLight;
class KarnanCamera;
class TerrainObject;

class KarnanEditor
{
public:
	enum class DetailsPanelTypes
	{
		NONE,
		CAMERA,
		GAMEOBJECT,
		POINT_LIGHT,
		MATERIAL,
		TERRAIN_OBJECT,
		PLAYER_OBJECT
	};	

	static KarnanEditor* Instance;

private:
	std::unique_ptr<KarnanMainGUI> _mainGUI;

	std::vector<IPanel*> _panels;

	DetailsPanelTypes _currentSelectedType = DetailsPanelTypes::NONE;
	std::shared_ptr<GameObject> _lastHighlightedGo = nullptr;
	void* _lastSelectedItem = nullptr;

	SceneDataObject _cachedScene;
	std::string _sceneName;

	bool _paused = false;
	bool _gamePlayed = false;

public:
	static KarnanEditor* StartupEditor();
	static void DestroyEditor();

	void Init();
	void Update();
	void Render(VkCommandBuffer commandBuffer);

	void CacheScene();
	void LoadCachedScene();

	GameObject* GetLastHighlightedGO();
	void ClearSelection();
	void SetLastHighlightedGO(GameObject* go);
	void SetPointLightAsSelected(PointLight* pointLight);
	void SetMaterialAsSelected(std::string filepath);
	void SetCameraAsSelected(KarnanCamera* camera);
	void SetTerrainObjectAsSelected(TerrainObject* terrain);

	DetailsPanelTypes GetCurrentSelectedType() const { return _currentSelectedType; };
	void* GetCurrentSelectedItem() const { return _lastSelectedItem; };

	bool Paused() { return _paused; };
	void SetPaused(bool paused) { _paused = paused; };

	bool PlayCalled() { return _gamePlayed; };
	void SetPlayed(bool played) { _gamePlayed = played; };

private:
	KarnanEditor();
	~KarnanEditor();

};