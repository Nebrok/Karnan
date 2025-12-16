#version 450

layout (location = 0) in vec3 fragColour;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 3) in vec2 texCoord;

layout (location = 0) out vec4 outColor; 

layout (set = 1, binding = 0) uniform sampler2D tex;

layout (push_constant) uniform Push
{
	mat4 modelMatrix;
} push;

vec3 directionToLight = vec3(0.5, 0.5, 0.5);
const float AMBIENT = 0.1;

void main()
{
	directionToLight = normalize(directionToLight);

	vec3 diffuseLight = vec3(1.0, 1.0, 1.0) * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

	vec3 color = texture(tex, texCoord).xyz;
	outColor = vec4(color * diffuseLight, 1.0);

}