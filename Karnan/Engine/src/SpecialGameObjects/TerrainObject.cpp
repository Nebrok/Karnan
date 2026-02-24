#include "TerrainObject.h"

#include <vector>
#include <../stb/stb_image.h>
#include <stdexcept>
#include <filesystem>

#include "../VertexBuffer.h"
#include "../MessagingSystem/Messages.h"
#include "../AssetManagement/AssetManager.h"

TerrainObject::TerrainObject()
	: GameObject("World Terrain")
{
}

TerrainObject::~TerrainObject()
{
}

void TerrainObject::Init()
{
	GameObject::Init();
	if (_collider == nullptr || _collider->Type != Collider::ColliderType::BOX)
	{
		_collider = std::shared_ptr<BoxCollider>(DBG_NEW BoxCollider());
		_colliderActive = true;
	}
	_collider->GameObject = this;

	BoxCollider* collider = static_cast<BoxCollider*>(_collider.get());
	collider->Extent.x = _width / 2;
	collider->Extent.y = 25.0f;
	collider->Extent.z = _length / 2;

	Tags.push_back("Terrain");


	GenerateTerrain();
	

}

void TerrainObject::Start()
{
	GameObject::Start();

}

void TerrainObject::Update(double deltaTime)
{
	GameObject::Update(deltaTime);
}

bool TerrainObject::InTerrainBounds(glm::vec2& coordinates)
{
	if (coordinates.x < Transform.Translation.x - _width / 2 || coordinates.x > Transform.Translation.x + _width / 2)
		return false;
	if (coordinates.y < Transform.Translation.z - _length / 2 || coordinates.y > Transform.Translation.z + _length / 2)
		return false;

	return true;
}

float TerrainObject::HeightAt(glm::vec2& coordinates)
{	
	float xLowerBound = Transform.Translation.x - _width / 2;
	float xUpperBound = Transform.Translation.x + _width / 2;

	float yLowerBound = Transform.Translation.z - _length / 2;
	float yUpperBound = Transform.Translation.z + _length / 2;

	if (coordinates.x < xLowerBound || coordinates.x > xUpperBound)
		throw std::runtime_error("Terrain height query coordinates out of bounds: X");
	if (coordinates.y < yLowerBound || coordinates.y > yUpperBound)
		throw std::runtime_error("Terrain height query coordinates out of bounds: Y");

	float xNormalised = InverseLerp(coordinates.x, xLowerBound, xUpperBound);
	float yNormalised = InverseLerp(coordinates.y, yLowerBound, yUpperBound);

	int x = (int)(_textureWidth * xNormalised);
	int y = (int)(_textureHeight * yNormalised);

	int index = x + y * _textureWidth;

	return _heights[index] * _maxHeight;
}

void TerrainObject::SetHeightMapFilepath(std::string filepath)
{
}

void TerrainObject::GenerateTerrain()
{
	int textureChannels;

	stbi_set_flip_vertically_on_load(true);
	stbi_uc* pixelData = stbi_load(_heightMapFilepath.c_str(), &_textureWidth, &_textureHeight, &textureChannels, STBI_rgb_alpha);

	if (!pixelData)
	{
		std::cout << "Unable to load texture with filepath: " << _heightMapFilepath << '\n';
		pixelData = stbi_load("assets/textures/NULL_TEXTURE.png", &_textureWidth, &_textureHeight, &textureChannels, STBI_rgb_alpha);
	}
	int imageSize = _textureWidth * _textureHeight * textureChannels;


	std::vector<VertexBuffer::Vertex>* vertices = DBG_NEW std::vector<VertexBuffer::Vertex>();
	std::vector<uint32_t>* indices = DBG_NEW std::vector<uint32_t>();

	float xDifference = (1.0f / _textureHeight) * _width;
	float yDifference = (1.0f / _textureHeight) * _length;
	float horizontalDistance = glm::sqrt(xDifference * xDifference + yDifference * yDifference);

	for (int j = 0; j < _textureHeight; j++)
	{
		for (int i = 0; i < _textureWidth; i++)
		{
			float u = i * (1.0f / _textureWidth);
			float v = j * (1.0f / _textureHeight);

			float height = (pixelData[(i + j * _textureWidth) * 4]) / 255.0f;
			_heights.push_back(height);
			VertexBuffer::Vertex newVertex;
			newVertex.position = { i * xDifference - (_width / 2.0f), height * _maxHeight, j * yDifference - (_length / 2.0f) };
			newVertex.normal = { 0.0f, 1.0f, 0.0f };
			newVertex.uv = { u, v };
			vertices->push_back(newVertex);
		}
	}

	for (int j = 0; j < _textureHeight; j++)
	{
		if (j == 0 || j == _textureHeight - 1)
			continue;
		for (int i = 0; i < _textureWidth; i++)
		{
			if (i == 0 || i == _textureWidth - 1)
				continue;

			VertexBuffer::Vertex& vertex = vertices->at(i + j * _textureWidth);


			VertexBuffer::Vertex vertexUp = vertices->at(i + (j - 1) * _textureWidth);
			VertexBuffer::Vertex vertexDown = vertices->at(i + (j + 1) * _textureWidth);

			VertexBuffer::Vertex vertexLeft = vertices->at((i - 1) + j * _textureWidth);
			VertexBuffer::Vertex vertexRight = vertices->at((i + 1) + j * _textureWidth);



			vertex.normal = { vertexLeft.position.y - vertexRight.position.y, horizontalDistance, vertexDown.position.x - vertexUp.position.x };
			vertex.normal = glm::normalize(vertex.normal);
		}
	}

	for (int j = 0; j < _textureHeight - 1; j++)
	{
		for (int i = 0; i < _textureWidth - 1; i++)
		{
			// Each i,j pair corressponds to a quad
			// AB
			// CD

			uint32_t vertexA = i + j * _textureWidth;
			uint32_t vertexB = (i + 1) + j * _textureWidth;
			uint32_t vertexC = i + (j + 1) * _textureWidth;
			uint32_t vertexD = (i + 1) + (j + 1) * _textureWidth;

			//triangle upper right
			indices->push_back(vertexA);
			indices->push_back(vertexB);
			indices->push_back(vertexD);

			//triangle lower left
			indices->push_back(vertexA);
			indices->push_back(vertexC);
			indices->push_back(vertexD);
		}
	}

	std::shared_ptr<AMCreateMeshMessage> createMessage = std::shared_ptr<AMCreateMeshMessage>(
		DBG_NEW AMCreateMeshMessage(Message::System::ASSET_MANAGER, "TerrainMesh", vertices, indices));

	std::unique_lock<std::mutex> messageQueueLock(AssetManager::Instance->MessageQueueMutex);
	AssetManager::Instance->QueueMessage(createMessage);
	messageQueueLock.unlock();

	_meshName = "TerrainMesh";
	stbi_image_free(pixelData);
}

float TerrainObject::InverseLerp(float x, float a, float b)
{
	return (x - a) / (b - a);
}

std::string TerrainObject::GetFilenameFromFilepath()
{
	std::filesystem::path filePath = { _heightMapFilepath };
	return std::string();
}
