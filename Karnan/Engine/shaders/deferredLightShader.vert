#version 450

layout (location = 0) in vec3 position; //will just be quad
layout (location = 1) in vec3 normal; //will just be quad
layout (location = 2) in vec2 uv; //will just be quad

layout (location = 0) out vec2 texCoords;

void main()
{
	texCoords = uv;
	gl_Position = vec4(position, 1.0);
}