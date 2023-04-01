#ifndef UTAH_ICG_OBJECT_H
#define UTAH_ICG_OBJECT_H

#include <vector>
#include "shader.h"

class Object {
public:
  Object(const std::vector<float> & vertices,
         const std::vector<float> & colours);
  ~Object();

  void main_loop();

private:
  void init_buffers(const std::vector<float> & vertices,
                    const std::vector<float> & colours);
  void destroy_buffers();
  void init_shader();

  GLuint vao_;
  GLuint vbo_pos_;
  GLuint vbo_clr_;
  std::shared_ptr<Shader> shader_;
  int32_t num_vertices_{};
};

#endif //UTAH_ICG_OBJECT_H
