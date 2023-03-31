#include "object.h"
#include "spdlog/spdlog-inl.h"
#include "OpenGL/gl3.h"


namespace {
  const char *vs_source[] = {
          R"(
#version 410 core

layout(location=0) in vec3 pos;

void main() {
  gl_Position = vec4(pos, 1.0);
}
)"};

  const GLchar *fs_source[] = {R"(
#version 410 core

layout (location=0) out vec4 frag_colour;

void main() {
  frag_colour=vec4( 1.0, 0.0, 0.0, 1.0);
}
)"};
}


Object::Object(const std::vector<float> &data) {
  init_shader();
  init_buffers(data);
}

Object::~Object() {
  destroy_buffers();
}


void Object::main_loop() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  shader_->use();
  glPointSize(10.0f);
  glDrawArrays(GL_POINTS, 0, 6);
}

void
Object::init_buffers(const std::vector<float> &data) {
  if (!shader_->is_good()) {
    return;
  }

  auto attr = shader_->get_attribute_location("pos");
  if (attr == -1) {
    spdlog::error("Invalid attribute location {}", attr);
  }

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);

  long sz = data.size() * sizeof(float);
  num_vertices_ = sz / 3;
  glBufferData(GL_ARRAY_BUFFER, sz, data.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(attr);
  glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);
}

void
Object::init_shader() {
  shader_ = std::make_shared<Shader>(vs_source, fs_source);
}

void
Object::destroy_buffers() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &vao_);
}