#version 460 core

#extension GL_ARB_shading_language_include : require
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec4 vertex_color;

layout (location = 0) out vec4 output_color;

layout (binding = 0) uniform time_info_t
{
	float time;
	float delta_time;
} time_info;

layout (binding = 1) uniform screen_info_t
{
	vec2 resolution;
} screen_info;

layout (binding = 2) uniform camera_info_t
{
	vec3 position;
	uint reserved;
	mat4 view;
	mat4 projection;
	mat4 view_projection;
	mat4 view_projection_inv;
	uint max_chunk_ray_steps;
	uint max_voxel_ray_steps;
} camera_info;

void main()
{
	vec4 world_position = vec4(vertex_position, 1.0);
	vec4 clip_position = camera_info.view_projection * world_position;

	output_color = vertex_color;

	gl_Position = clip_position;
}
