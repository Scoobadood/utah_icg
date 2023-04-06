#version 410 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;

uniform mat4 model_view;
uniform mat4 project;

out vec3 frag_position;
out vec3 frag_normal;

void main() {
  mat3 mvt;
  mvt[0][0] = model_view[0][0];
  mvt[0][1] = model_view[0][1];
  mvt[0][2] = model_view[0][2];
  mvt[1][0] = model_view[1][0];
  mvt[1][1] = model_view[1][1];
  mvt[1][2] = model_view[1][2];
  mvt[2][0] = model_view[2][0];
  mvt[2][1] = model_view[2][1];
  mvt[2][2] = model_view[2][2];

  mat3 imvt = inverse(mvt);
  mat3 timvt = transpose(imvt);

  frag_normal = timvt * normal;
  frag_position = (model_view * vec4(pos, 1.0)).xyz;
  gl_Position = project * model_view * vec4(pos, 1.0);
}