#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec4 VertexColor;

layout(location = 0) out vec4 OutputColor;

layout(binding = 0) uniform TimeInfoUniform {
  float Time;
  float DeltaTime;
}
TimeInfo;

layout(binding = 1) uniform ScreenInfoUniform {
  float Width;
  float Height;
}
ScreenInfo;

layout(binding = 2) uniform CameraInfoUniform {
  vec3 Position;
  float Reserved;
  mat4 View;
  mat4 Projection;
  mat4 ViewProjection;
  mat4 ViewProjectionInv;
}
CameraInfo;

void main() {
  vec4 WorldPosition = vec4(VertexPosition, 1.0);
  vec4 ClipPosition = CameraInfo.ViewProjection * WorldPosition;

  OutputColor = VertexColor;

  gl_Position = ClipPosition;
}
