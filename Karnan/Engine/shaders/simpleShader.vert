#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragColour;
layout (location = 1) out vec2 texCoord;

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionViewMatrix;
} ubo;

layout (push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

vec3 directionToLight = vec3(0, 1, 0);
const float AMBIENT = 0.01;

void main()
{
	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);
	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
	
	float lightIntensity = AMBIENT + max(dot(normalWorldSpace, directionToLight), 0);

	texCoord = uv;
	fragColour = vec3(lightIntensity);
}