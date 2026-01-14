#version 450

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec3 fragNormalWorld;
layout (location = 2) in vec2 texCoord;

layout (set = 1, binding = 0) uniform sampler2D tex;

layout (location = 0) out vec3 gBufferPosition;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec4 gBufferAlbedo;

void main()
{
	gBufferPosition = fragPosWorld;
	gBufferNormal = normalize(fragNormalWorld);
	gBufferAlbedo = vec4(texture(tex, texCoord).rgb, 1.0);
}