#version 450 core

///////////////////////////////////////////////////////////
// Vertex Inputs
///////////////////////////////////////////////////////////

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec2 InputTextureMap;
layout (location = 2) in vec2 InputTextureUv;
layout (location = 3) in vec4 InputColorWeight;

///////////////////////////////////////////////////////////
// Vertex Outputs
///////////////////////////////////////////////////////////

layout (location = 0) out Vertex
{
  vec3 Position;
  vec2 Uv;
  vec4 Color;
} vertex;

///////////////////////////////////////////////////////////
// Uniforms
///////////////////////////////////////////////////////////

uniform mat4 UniformProjectionMatrix;
uniform mat4 UniformViewMatrix;
uniform mat4 UniformModelMatrix;

///////////////////////////////////////////////////////////
// Entry point
///////////////////////////////////////////////////////////

void main()
{
  vertex.Position = vec3(UniformModelMatrix * vec4(InputPosition, 1.0));
  vertex.Uv = InputTextureMap;
  vertex.Color = vec4(InputTextureMap, 0.0, 1.0);

  gl_Position = UniformProjectionMatrix * UniformViewMatrix * UniformModelMatrix * vec4(InputPosition, 1.0);
}