#version 460 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec2 InputUv;
layout (location = 2) in vec4 InputColor;
layout (location = 3) flat in uint InputEntityId;

layout (location = 0) out vec4 OutputColor;
layout (location = 1) out uint OutputId;

layout (binding = 0) uniform sampler2D SamplerAlbedo;

void main()
{
	OutputColor = texture(SamplerAlbedo, InputUv);
	OutputId = InputEntityId;
}