@vertex
#version 450 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec2 InputTextureMap;
layout (location = 2) in vec2 InputTextureUv;
layout (location = 3) in uint InputColorWeight;

layout (location = 0) out Vertex
{
  vec3 Position;
  vec4 Color;
} vertex;

uniform mat4 UniformProjectionMatrix;
uniform mat4 UniformViewMatrix;
uniform mat4 UniformModelMatrix;

void main()
{
  vertex.Position = (UniformModelMatrix * vec4(InputPosition, 1.0)).xyz;
  vertex.Color = vec4(InputTextureMap, 0.0, 1.0);
  gl_Position = UniformProjectionMatrix * UniformViewMatrix * UniformModelMatrix * vec4(InputPosition, 1.0);
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