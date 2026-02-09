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

const float AMBIENT = 0.4;

void main()
{
	vec3 matDiffuseColor = texture(gBufferAlbedo, texCoords).xyz;
	float specularity = 1-texture(gBufferAlbedo, texCoords).a;
	vec3 normal = texture(gBufferNormal, texCoords).xyz;
	vec3 worldPosition = texture(gBufferPosition, texCoords).xyz;

	vec3 normalisedCameraDirection = normalize(lightUbo.cameraPos.xyz - worldPosition);

	vec3 diffuseLumin = vec3(0,0,0);
	vec3 specularLumin = vec3(0,0,0);
	
	for (int i = 0; i < lightUbo.numberLights.x; i++)
	{
		vec3 lightDiffuse = vec3(0,0,0);
		vec3 lightSpecular = vec3(0,0,0);

		vec3 directionToLight = normalize(lightUbo.lightPositions[i].xyz - worldPosition);

		float difIntensity = max(dot(directionToLight, normal), 0.0);
		if (difIntensity > 0)
		{
			vec3 diffusePart = difIntensity * lightUbo.lightColours[i].xyz * matDiffuseColor;
			float distance = length(lightUbo.lightPositions[i].xyz - worldPosition);

			float attenuation = lightUbo.lightIntensities[i].x / (lightUbo.lightAttentuations[i].x + lightUbo.lightAttentuations[i].y * distance + lightUbo.lightAttentuations[i].z * pow(distance, 2));

			lightDiffuse += diffusePart * attenuation;

			vec3 halfVector = normalize((directionToLight + normalisedCameraDirection) / 2);
			float initialBrightness = max(dot(halfVector, normal), 0.0);

			float totalBrightness = pow(initialBrightness, 160 * specularity);

			lightSpecular = totalBrightness * lightUbo.lightColours[i].xyz * attenuation * specularity; //* matDiffuseColor;

		}

		diffuseLumin += lightDiffuse;
		specularLumin += lightSpecular;


	}
	
	outColor = vec4((matDiffuseColor * AMBIENT) + diffuseLumin + specularLumin, 1);
	//outColor = vec4(texture(gBufferPosition, texCoords).rgb, 1);
	//outColor = vec4(texture(gBufferNormal, texCoords).rgb, 1);
	//outColor = vec4(texture(gBufferAlbedo, texCoords).rgb, 1);
}