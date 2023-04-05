#include "object.h"
#include "spdlog/spdlog-inl.h"
#include "mesh.h"
#include "gl_common.h"

GLenum glerr;
#define CHECK_GL_ERROR(txt)   \
if ((glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, glerr); \
throw std::runtime_error("GLR"); \
} \

namespace {
  const char *vs_source[] = {
          R"(
#version 410 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;

out vec3 position;
smooth out vec3 int_normal;

void main() {
  mat4 view = mat4(-0.70710678118, 0.0, 0.70710678118, 0.0,  // 1. column
                   0.0, 1.0, 0.0, 0.0,  // 2. column
                   -0.70710678118, 0.0, -0.70710678118, 0.0,  // 3. column
                   0.0, 0.0, 0.0, 1.0);
  vec4 p4 = view * vec4(pos, 1.0);

  gl_Position = p4;
  position = p4.xyz;
  int_normal = normal;
}
)"};

  const GLchar *fs_source[] = {R"(
#version 410 core

layout (location=0) out vec4 frag_colour;

uniform float light_int;
uniform float light_amb;
uniform vec3 light_pos;
uniform float alpha;

in vec4 colour;
in vec3 int_normal;
in vec3 position;

void main() {

  vec3 nn = normalize(int_normal);
  // Calc diffuse lighting geometry component
  vec3 light_dir = normalize(light_pos - position.xyz);
  float n_dot_w = max(0,dot(light_dir, nn));

  // Calc specular lighting Phong
  vec3 look = -normalize(position.xyz);
  vec3 r = -reflect(light_dir, nn);
  float r_dot_v = max(0,dot(r, look));
  float spec_coeff = pow(r_dot_v, alpha);

  vec3 light = light_int * n_dot_w * (vec3(1,0,0) + vec3(1) * spec_coeff ) + light_amb;
  frag_colour=vec4(int_normal, 1);
}
)"};
}

Object::Object(const std::string &file_name,
               bool include_normals,
               bool include_tex_coords) {
  init_shader();
  if (!shader_->is_good()) {
    return;
  }
  CHECK_GL_ERROR("shader build");

  shader_->use();

  auto pos_attr = shader_->get_attribute_location("pos");
  if (pos_attr == -1) {
    spdlog::error("Invalid attribute location pos:{}", pos_attr);
    return;
  }

  uint32_t norm_attr=-1;
  if( include_normals) {
    norm_attr = shader_->get_attribute_location("normal");
    if (norm_attr == -1) {
      spdlog::warn("Missing attribute location norm. Disabling normals.");
      include_normals = false;
    }
  }

  uint32_t tx_attr = -1;
  load_obj(file_name, vao_, vbo_, ebo_,
           num_elements_, pos_attr,
           include_normals, norm_attr,
           include_tex_coords, tx_attr);
  CHECK_GL_ERROR("load obj");

  shader_->set_uniform("light_int", 1.0f);
  shader_->set_uniform("light_amb", 0.01f);
  shader_->set_uniform("light_pos", glm::vec3(1,1,1));
  shader_->set_uniform("alpha", 1000.0f);
}


Object::~Object() {
  destroy_buffers();
}

void Object::main_loop() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vao_);

  shader_->use();

  float posx = sin(angle_);
  float posz = cos(angle_);
  angle_ += 0.01f;
  shader_->set_uniform("light_pos", glm::vec3(posx, 1.0, posz));

  glPointSize(5.0f);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);
}

void
Object::init_shader() {
  shader_ = std::make_shared<Shader>(vs_source, fs_source);
}

void
Object::destroy_buffers() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteBuffers(1, &vao_);
}
