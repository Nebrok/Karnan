#include "GameObject.h"

GameObject::GameObject(KarnanDevice& device)
	: _karnanDevice(device)
{
}

GameObject::~GameObject()
{
}

void GameObject::Init()
{
	VertexBuffer::Vertex vertex1{};
	vertex1.position = { 0.f, -0.5f, 0.f };
	vertex1.colour = { 1.0f, 0.2f, 0.2f };
	VertexBuffer::Vertex vertex2{};
	vertex2.position = { 0.5f, 0.5f, 0.f };
	vertex2.colour = { 0.2f, 1.0f, 0.2f };
	VertexBuffer::Vertex vertex3{};
	vertex3.position = { -0.5f, 0.5f, 0.f };
	vertex3.colour = { 0.2f, 0.2f, 1.0f };

	std::vector<VertexBuffer::Vertex> vertices
	{
		vertex1,
		vertex2,
		vertex3
	};

	std::unique_ptr<BasicMesh> mesh(new BasicMesh(_karnanDevice, vertices, std::vector<uint32_t>()));
	_mesh = move(mesh);

}

void GameObject::Update(float deltaTime)
{
	Transform.Rotation.y += 1 * deltaTime;
}

void GameObject::Render(VkCommandBuffer commandBuffer)
{
	_mesh->Bind(commandBuffer);
	_mesh->Draw(commandBuffer);
}
