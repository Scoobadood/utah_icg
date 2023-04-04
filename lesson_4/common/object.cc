#include "object.h"
#include "spdlog/spdlog-inl.h"
#include "mesh.h"

namespace {
  const char *vs_source[] = {
          R"(
#version 410 core

layout(location=11) in vec3 pos;
layout(location=7) in vec4 clr;

out vec4 colour;

void main() {
  gl_Position = vec4(pos, 1.0);
  colour = clr;
}
)"};

  const GLchar *fs_source[] = {R"(
#version 410 core

layout (location=0) out vec4 frag_colour;

in vec4 colour;

void main() {
  frag_colour=colour;
}
)"};
}


Object::Object(const std::string& file_name,
       bool include_normals,
       bool include_tex_coords){
  init_shader();
  if (!shader_->is_good()) {
    return;
  }

  auto pos_attr = shader_->get_attribute_location("pos");
  if( pos_attr == -1) {
    spdlog::error("Invalid attribute location pos:{}", pos_attr);
    return;
  }

  uint32_t norm_attr=0, tx_attr=0;
  load_obj(file_name, vao_, vbo_, ebo_,
           num_elements_,

           pos_attr,
           false, norm_attr,
           false, tx_attr);
}


Object::~Object() {
  destroy_buffers();
}


void Object::main_loop() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(vao_);

  shader_->use();
  glPointSize(10.0f);

  glDrawElements(GL_POINTS, num_elements_, GL_INT, (void *)nullptr);
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
