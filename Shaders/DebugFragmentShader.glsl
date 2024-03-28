#version 460 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec4 InputColor;

layout (location = 0) out vec4 OutputColor;

void main()
{
	OutputColor = InputColor;
}