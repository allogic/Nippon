#version 450 core

///////////////////////////////////////////////////////////
// Fragment Inputs
///////////////////////////////////////////////////////////

layout (location = 0) in Vertex
{
  vec3 Position;
  vec4 Color;
} vertex;

///////////////////////////////////////////////////////////
// Fragment Outputs
///////////////////////////////////////////////////////////

layout (location = 0) out vec4 OutputColor;

///////////////////////////////////////////////////////////
// Entry point
///////////////////////////////////////////////////////////

void main()
{
  OutputColor = vertex.Color;
}