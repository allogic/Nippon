@vertex
#version 450 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec4 InputColor;

layout (location = 0) out Vertex
{
  vec3 Position;
  vec4 Color;
} vertex;

uniform mat4 UniformProjectionMatrix;
uniform mat4 UniformViewMatrix;

void main()
{
  vertex.Position = InputPosition;
  vertex.Color = InputColor;
  gl_Position = UniformProjectionMatrix * UniformViewMatrix * vec4(InputPosition, 1.0);
}

@fragment
#version 450 core

layout (location = 0) in Vertex
{
  vec3 Position;
  vec4 Color;
} vertex;

layout (location = 0) out vec4 OutputColor;

void main()
{
  OutputColor = vertex.Color;
}