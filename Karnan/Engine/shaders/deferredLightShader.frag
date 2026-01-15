#version 450

#define MAX_LIGHTS 128

layout (location = 0) in vec2 texCoords;

layout (set = 0, binding = 0) uniform LightingUbo
{
	vec4 cameraPos;
	vec4 lightPositions[MAX_LIGHTS];
	vec4 lightColours[MAX_LIGHTS];
	vec4 lightAttentuations[MAX_LIGHTS];
	vec4 lightIntensities[MAX_LIGHTS];
	vec4 numberLights;
} lightUbo;


layout (set = 1, binding = 0) uniform sampler2D gBufferPosition;
layout (set = 1, binding = 1) uniform sampler2D gBufferNormal;
layout (set = 1, binding = 2) uniform sampler2D gBufferAlbedo;

layout (location = 0) out vec4 outColor; 

void main()
{
	vec3 toCamera = lightUbo.cameraPos.rgb - texture(gBufferPosition, texCoords).rgb;
	float cameraAngle = dot(normalize(toCamera), texture(gBufferNormal, texCoords).rgb);

	outColor = vec4(texture(gBufferAlbedo, texCoords).rgb * cameraAngle, 1.0);
}