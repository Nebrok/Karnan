#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragPosWorld;
layout (location = 1) out vec3 fragNormalWorld;
layout (location = 2) out vec2 texCoord;

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionViewMatrix;
} ubo;

layout (push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	fragPosWorld = vec3(push.modelMatrix * vec4(position, 1.0));
	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	texCoord = uv;

	gl_Position = ubo.projectionViewMatrix * vec4(fragPosWorld, 1.0);
}