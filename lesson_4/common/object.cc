#include "object.h"
#include "spdlog/spdlog-inl.h"

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


Object::Object(const std::vector<float> &vertices,
               const std::vector<float> &colours) {
  init_shader();
  init_buffers(vertices, colours);
}

Object::~Object() {
  destroy_buffers();
}


void Object::main_loop() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  shader_->use();
  glPointSize(10.0f);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void
Object::init_buffers(const std::vector<float> &vertices,
                     const std::vector<float> &colours) {
  if (!shader_->is_good()) {
    return;
  }

  auto pos_attr = shader_->get_attribute_location("pos");
  auto clr_attr = shader_->get_attribute_location("clr");
  if (pos_attr == -1 || clr_attr == -1) {
    spdlog::error("Invalid attribute location pos:{}, clr:{}", pos_attr, clr_attr);
    return;
  }

  // VAO
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);


  long sz = vertices.size() * sizeof(float);
  num_vertices_ = sz / 3;

  // Vertex locations
  GLuint buffs[2];
  glGenBuffers(2, buffs);

  vbo_pos_ = buffs[0];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
  glBufferData(GL_ARRAY_BUFFER, sz, vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(pos_attr);
  glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

  // Vertex colours
  vbo_clr_ = buffs[1];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_clr_);
  glBufferData(GL_ARRAY_BUFFER,num_vertices_*4, colours.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(clr_attr);
  glVertexAttribPointer(clr_attr, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
}

void
Object::init_shader() {
  shader_ = std::make_shared<Shader>(vs_source, fs_source);
}

void
Object::destroy_buffers() {
  glDeleteBuffers(1, &vbo_pos_);
  glDeleteBuffers(1, &vbo_clr_);
  glDeleteBuffers(1, &vao_);
}
