#version 450

layout (location = 0) in vec2 position;
layout (location = 2) in vec3 normal;

layout (push_constant) uniform Push
{
	vec2 offset;
} push;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
}