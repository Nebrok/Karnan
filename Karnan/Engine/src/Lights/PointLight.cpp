#include "PointLight.h"
#include "../AssetManagement/AssetManager.h"

PointLight::PointLight()
	: GameObject("Point Light")
{
	_renderable = false;
	
}

void PointLight::Init()
{
	Tags.push_back("Point Light");
}

void PointLight::Update(double deltaTime)
{
	_material = AssetManager::Instance->GetMaterial(_materialName);
	if (_material != nullptr)
	{
		_renderable = true;
	}
}
