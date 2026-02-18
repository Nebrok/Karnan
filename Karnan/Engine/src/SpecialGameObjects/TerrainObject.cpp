#include "TerrainObject.h"

#include <vector>
#include <../stb/stb_image.h>

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
	if (_collider == nullptr)
	{
		_collider = std::shared_ptr<SphereCollider>(DBG_NEW SphereCollider());
		_colliderActive = false;
	}
	_collider->GameObject = this;

	int textureWidth;
	int textureHeight;
	int textureChannels;

	stbi_set_flip_vertically_on_load(true);
	stbi_uc* pixelData = stbi_load(_filepath.c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

	if (!pixelData)
	{
		std::cout << "Unable to load texture with filepath: " << _filepath << '\n';
		pixelData = stbi_load("assets/textures/NULL_TEXTURE.png", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
	}
	int imageSize = textureWidth * textureHeight * textureChannels;


	std::vector<VertexBuffer::Vertex>* vertices = DBG_NEW std::vector<VertexBuffer::Vertex>();
	std::vector<uint32_t>* indices = DBG_NEW std::vector<uint32_t>();

	float xDifference = (1.0f / textureWidth) * _width;
	float yDifference = (1.0f / textureHeight) * _length;

	for (int j = 0; j < textureHeight; j++)
	{
		for (int i = 0; i < textureWidth; i++)
		{
			float u = i * (1.0f / textureWidth);
			float v = j * (1.0f / textureHeight);


			float height = (pixelData[(i + j * textureWidth) * 4]) / 255.0f;
			VertexBuffer::Vertex newVertex;
			newVertex.position = { i * xDifference - ( _width / 2.0f), height * _maxHeight, j * yDifference - (_length / 2.0f) };
			newVertex.normal = { 0.0f, 1.0f, 0.0f };
			newVertex.uv = { u, v };
			vertices->push_back(newVertex);
		}
	}

	for (int j = 0; j < textureHeight -1; j++)
	{
		for (int i = 0; i < textureWidth -1; i++)
		{
			// Each i,j pair corressponds to a quad
			// AB
			// CD

			uint32_t vertexA = i + j * textureWidth;
			uint32_t vertexB = (i + 1) + j * textureWidth;
			uint32_t vertexC = i + (j + 1) * textureWidth;
			uint32_t vertexD = (i + 1) + (j + 1) * textureWidth;

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

void TerrainObject::Update(double deltaTime)
{
}
