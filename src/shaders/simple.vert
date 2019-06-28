#version 450 core

in vec3 vtx_position;
in vec3 vtx_normal;
in vec2 vtx_texcoord;

uniform mat4 mat_proj;
uniform mat4 mat_view;
uniform mat4 mat_model;
uniform mat3 mat_normal;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec2 out_texcoord;
layout(location = 2) out vec3 out_normal;

void main() {
  gl_Position = mat_proj * mat_view * mat_model * vec4(vtx_position, 1.0);
  out_position = (mat_model * vec4(vtx_position, 1.0)).xyz;
  out_texcoord = vtx_texcoord;
  out_normal = normalize(mat_normal * vtx_normal);
}