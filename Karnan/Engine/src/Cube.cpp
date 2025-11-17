#include "Cube.h"

#include <iostream>

Cube::Cube(const char* objectName)
	: GameObject(objectName)
{
	std::vector<VertexBuffer::Vertex> vertices
	{
		// Front Face
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}},

		// Back Face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},

		// Top Face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}},

		// Bottom Face
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},

		//Left Face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}},

		//Right
		{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}}
	};

	std::vector<uint32_t> indices
	{
		// Front face
		0, 2, 1, 2, 0, 3,
		// Back face
		4, 6, 5, 6, 4, 7,
		// Bottom face
		8, 10, 9, 10, 8, 11,
		// Top face
		12, 14, 13, 14, 12, 15,
		// Left face
		16, 18, 17, 18, 16, 19,
		// Right face
		20, 22, 21, 22, 20, 23
	};

	std::cout << "VertexCount : " << vertices.size() << "\n";
	std::cout << "IndexCount : " << indices.size() << "\n";

	std::unique_ptr<BasicMesh> mesh(DBG_NEW BasicMesh(_karnanDevice, vertices, indices));
	_mesh = move(mesh);

	_material = std::make_unique<KarnanMaterial>();
	_material->Init();
	_material->CreateTextureInSlot(0, { "textures/texture.jpg" });
	_material->CreateImageInfos();

	SetRenderable(true);
}

Cube::~Cube()
{
}

void Cube::Update(double deltaTime)
{
	Transform.Rotation.y += (float)(1.f * deltaTime);
}
