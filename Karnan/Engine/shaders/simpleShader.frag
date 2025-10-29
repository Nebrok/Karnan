#version 450

layout (location = 0) out vec4 outColor; 

layout (push_constant) uniform Push
{
	vec2 offset;
} push;

void main()
{
	outColor = vec4(1.0, 0.1, 0.1, 1.0);
}