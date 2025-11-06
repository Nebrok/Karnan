#version 450

layout (location = 0) in vec3 fragColour;
layout (location = 1) in vec2 texCoord;

layout (location = 0) out vec4 outColor; 

layout (set = 1, binding = 0) uniform sampler2D tex;

layout (push_constant) uniform Push
{
	mat4 modelMatrix;
} push;

void main()
{
	vec3 color = texture(tex, texCoord).xyz;
	outColor = vec4(color, 1.0);
}