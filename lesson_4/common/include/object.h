#ifndef UTAH_ICG_OBJECT_H
#define UTAH_ICG_OBJECT_H

#include <vector>
#include "shader.h"

class Object {
public:
  Object(const std::vector<float> & data);
  ~Object();

  void main_loop();

private:
  void init_buffers(const std::vector<float> & data);
  void destroy_buffers();
  void init_shader();

  GLuint vao_;
  GLuint vbo_;
  std::shared_ptr<Shader> shader_;
  int32_t num_vertices_;
};

#endif //UTAH_ICG_OBJECT_H
