#ifndef UTAH_ICG_OBJECT_H
#define UTAH_ICG_OBJECT_H

#include <vector>
#include "shader.h"

#ifdef __APPLE__
#include "OpenGL/gl3.h"
#include "texture.h"

#else
#include "GL/glew.h"
#include "GL/glext.h"
#include "GL/gl.h"
#endif

#include <memory>

class Object {
public:
  Object(const std::string& file_name,
         bool include_normals,
         bool include_tex_coords);

  ~Object();

  void main_loop();

  void spot_on_off(bool on_off) {spot_on_ = on_off;}
  void big_light_on(bool on_off) {big_light_on_ = on_off;}

private:
  void destroy_buffers();
  void init_shader();

  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  GLuint num_elements_;
  std::shared_ptr<Texture> texture_;
  std::shared_ptr<Shader> shader_;
  std::shared_ptr<Shader> textured_shader_;
  float head_1_angle_ = 0.0f;
  float head_2_angle_ = 0.0f;
  float head_3_angle_ = 0.0f;
  float view_angle_ = 0.0f;
  bool spot_on_ = true;
  bool big_light_on_ = false;
};

#endif //UTAH_ICG_OBJECT_H
