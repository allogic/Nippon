#version 450 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec2 InputTextureMap;
layout (location = 2) in vec2 InputTextureUv;
layout (location = 3) in vec4 InputColorWeight;
layout (location = 4) in uint InputActorId;

layout (location = 0) out vec3 OutputPosition;
layout (location = 1) out vec2 OutputUv;
layout (location = 2) out vec4 OutputColor;
layout (location = 3) flat out uint OutputActorId;

uniform mat4 UniformProjectionMatrix;
uniform mat4 UniformViewMatrix;
uniform mat4 UniformModelMatrix;

void main()
{
	OutputPosition = vec3(UniformModelMatrix * vec4(InputPosition, 1.0));
	OutputUv = InputTextureMap;
	OutputColor = InputColorWeight;
	OutputActorId = InputActorId;

	gl_Position = UniformProjectionMatrix * UniformViewMatrix * UniformModelMatrix * vec4(InputPosition, 1.0);
}
