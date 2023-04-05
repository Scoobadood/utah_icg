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

uniform float light_int;
uniform float light_amb;
uniform vec3 light_pos;

out vec4 colour;

void main() {
  mat4 view = mat4(-0.70710678118, 0.0, 0.70710678118, 0.0,  // 1. column
                   0.0, 1.0, 0.0, 0.0,  // 2. column
                   -0.70710678118, 0.0, -0.70710678118, 0.0,  // 3. column
                   0.0, 0.0, 0.0, 1.0);
  vec4 p4 = view * vec4(pos, 1.0);

  vec3 light_dir = normalize(light_pos - pos);
  float dp = max(0,dot(light_dir, normal));
  vec3 light = vec3(light_int * dp + light_amb);
  colour  = vec4(light, 1);

  gl_Position = p4;
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

std::string name_for_type(GLenum type) {
  switch (type) {
    case GL_BYTE:
      return "GL_BYTE";
    case GL_UNSIGNED_BYTE:
      return "GL_UNSIGNED_BYTE";
    case GL_SHORT:
      return "GL_SHORT";
    case GL_UNSIGNED_SHORT:
      return "GL_UNSIGNED_SHORT";
    case GL_INT:
      return "GL_INT";
    case GL_UNSIGNED_INT:
      return "GL_UNSIGNED_INT";
    case GL_FLOAT:
      return "GL_FLOAT";
    case GL_DOUBLE:
      return "GL_DOUBLE";
    case GL_FLOAT_VEC2:
      return "GL_FLOAT_VEC2";
    case GL_FLOAT_VEC3:
      return "GL_FLOAT_VEC3";
    case GL_FLOAT_VEC4:
      return "GL_FLOAT_VEC4";
    case GL_INT_VEC2:
      return "GL_INT_VEC2";
    case GL_INT_VEC3:
      return "GL_INT_VEC3";
    case GL_INT_VEC4:
      return "GL_INT_VEC4";
    case GL_BOOL:
      return "GL_BOOL";
    case GL_BOOL_VEC2:
      return "GL_BOOL_VEC2";
    case GL_BOOL_VEC3:
      return "GL_BOOL_VEC3";
    case GL_BOOL_VEC4:
      return "GL_BOOL_VEC4";
    case GL_FLOAT_MAT2:
      return "GL_FLOAT_MAT2";
    case GL_FLOAT_MAT3:
      return "GL_FLOAT_MAT3";
    case GL_FLOAT_MAT4:
      return "GL_FLOAT_MAT4";
    case GL_SAMPLER_1D:
      return "GL_SAMPLER_1D";
    case GL_SAMPLER_2D:
      return "GL_SAMPLER_2D";
    case GL_SAMPLER_3D:
      return "GL_SAMPLER_3D";
    default:
      return fmt::format("UNNOWN ({:04x})", type);
  }
}
void dump_shader(const std::shared_ptr<Shader> &shader) {
  char buff[1024];
  int32_t count;
  GLenum type;
  GLint size;
  GLsizei length;
  glGetProgramiv(shader->id(), GL_ACTIVE_ATTRIBUTES, &count);
  for( auto i=0; i<count; ++i) {
    glGetActiveAttrib(shader->id(), i, 1024,&length,&size,&type,buff);
    spdlog::info( "Attr {:2}: {}  {}, {}", i, buff, size, name_for_type(type));
  }
  glGetProgramiv(shader->id(), GL_ACTIVE_UNIFORMS, &count);
  for( auto i=0; i<count; ++i) {
    glGetActiveUniform(shader->id(), i, 1024,&length,&size,&type,buff);
    spdlog::info( "Unif {:2}: {}  {}, {}", i, buff, size, name_for_type(type));
  }
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
  shader_->set_uniform("light_amb", 0.2f);
  shader_->set_uniform("light_pos", glm::vec3(1,1,1));
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
  angle_ += 0.05f;
  shader_->set_uniform("light_pos", glm::vec3(posx,0.75,posz));

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
