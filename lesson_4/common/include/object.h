#ifndef UTAH_ICG_OBJECT_H
#define UTAH_ICG_OBJECT_H

#include <vector>
#include "shader.h"

#ifdef __APPLE__
#include "OpenGL/gl3.h"
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

private:
  void destroy_buffers();
  void init_shader();

  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  GLuint num_elements_;
  std::shared_ptr<Shader> shader_;
};

#endif //UTAH_ICG_OBJECT_H
