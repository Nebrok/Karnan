#version 450

#define MAX_LIGHTS 16

layout (location = 0) in vec3 fragColour;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;
layout (location = 3) in vec2 texCoord;
layout (location = 4) in vec3 vecToEye;

layout (location = 0) out vec4 outColor; 

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projectionViewMatrix;
	vec4 cameraPos;
	vec4 lightPositions[MAX_LIGHTS];
	vec4 lightColours[MAX_LIGHTS];
	vec4 lightAttentuations[MAX_LIGHTS];
	vec4 lightIntensities[MAX_LIGHTS];
	vec4 numberLights;
} ubo;

layout (set = 1, binding = 0) uniform sampler2D tex;

layout (push_constant) uniform Push
{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

const float AMBIENT = 0.1;

void main()
{
	vec3 matDiffuseColor = texture(tex, texCoord).xyz;

	vec3 diffuseLumin = vec3(0,0,0);
	vec3 specularLumin = vec3(0,0,0);
	
	vec3 vectorToEye = normalize(vecToEye);
	vec3 normalisedNormal = normalize(fragNormalWorld);

	for (int i = 0; i < ubo.numberLights.x; i++)
	{
		vec3 lightDiffuse = vec3(0,0,0);
		vec3 lightSpecular = vec3(0,0,0);

		vec3 directionToLight = normalize(ubo.lightPositions[i].xyz - fragPosWorld);

		float difIntensity = max(dot(directionToLight, normalisedNormal), 0.0);
		if (difIntensity > 0)
		{
			vec3 diffusePart = difIntensity * ubo.lightColours[i].xyz * matDiffuseColor;
			float distance = length(ubo.lightPositions[i].xyz - fragPosWorld);

			float attenuation = 1 / (ubo.lightAttentuations[i].x + ubo.lightAttentuations[i].y * distance + ubo.lightAttentuations[i].z * pow(distance, 2));

			lightDiffuse += diffusePart * attenuation;

			vec3 halfVector = normalize((directionToLight + vectorToEye) / 2);
			float initialBrightness = max(dot(halfVector, normalisedNormal), 0.0);

			float totalBrightness = initialBrightness;
			for (int j = 0; j < 80; j++)
			{
				totalBrightness *= initialBrightness;
			}
			
			lightSpecular = totalBrightness * ubo.lightColours[i].xyz * attenuation * matDiffuseColor;

		}

		diffuseLumin += lightDiffuse;
		specularLumin += lightSpecular;

	}


	outColor = vec4((matDiffuseColor * AMBIENT) + diffuseLumin + specularLumin, 1);

}