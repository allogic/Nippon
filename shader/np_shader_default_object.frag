#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec4 InputColor;

layout(location = 0) out vec4 OutputColor;

void main() {
  OutputColor = InputColor;
}
