#version 450 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec4 InputColor;

layout (location = 0) out vec3 OutputPosition;
layout (location = 1) out vec4 OutputColor;

uniform mat4 UniformProjectionMatrix;
uniform mat4 UniformViewMatrix;

void main()
{
	OutputPosition = InputPosition;
	OutputColor = InputColor;

	gl_Position = UniformProjectionMatrix * UniformViewMatrix * vec4(InputPosition, 1.0);
}
