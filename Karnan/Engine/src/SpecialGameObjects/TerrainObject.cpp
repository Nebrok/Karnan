#include "TerrainObject.h"

#include <vector>

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

	std::vector<VertexBuffer::Vertex>* vertices = DBG_NEW std::vector<VertexBuffer::Vertex>();
	std::vector<uint32_t>* indices = DBG_NEW std::vector<uint32_t>();

	int lengthVertexCount = 100;
	int widthVertexCount = 100;

	for (int j = 0; j < lengthVertexCount; j++)
	{
		for (int i = 0; i < widthVertexCount; i++)
		{
			float u = i * (1.0f / widthVertexCount);
			float v = j * (1.0f / lengthVertexCount);

			VertexBuffer::Vertex newVertex;
			newVertex.position = { i - widthVertexCount / 2.0f, (rand() % 100) / 10.0f, j - lengthVertexCount / 2.0f};
			newVertex.normal = { 0.0f, 1.0f, 0.0f };
			newVertex.uv = { u, v };
			vertices->push_back(newVertex);
		}
	}

	for (int j = 0; j < lengthVertexCount -1; j++)
	{
		for (int i = 0; i < widthVertexCount -1; i++)
		{
			// Each i,j pair corressponds to a quad
			// AB
			// CD

			uint32_t vertexA = i + j * lengthVertexCount;
			uint32_t vertexB = (i + 1) + j * lengthVertexCount;
			uint32_t vertexC = i + (j + 1) * lengthVertexCount;
			uint32_t vertexD = (i + 1) + (j + 1) * lengthVertexCount;

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

}

void TerrainObject::Update(double deltaTime)
{
}
