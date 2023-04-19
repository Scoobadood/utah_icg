#version 410 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

out vec3 frag_position;
out vec2 frag_tex_coord;
out vec3 frag_normal;

void main() {
  mat4 model_view = view * model;
  mat3 mvt = mat3(model_view);

  mat3 i_mvt = inverse(mvt);
  mat3 t_i_mvt = transpose(i_mvt);

  frag_normal = t_i_mvt * normal;
  frag_position = (model_view * vec4(pos, 1.0)).xyz;
  gl_Position = project * model_view * vec4(pos, 1.0);
  frag_tex_coord = tex_coords;
}