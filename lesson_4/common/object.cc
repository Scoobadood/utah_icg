#include "object.h"
#include "spdlog/spdlog-inl.h"
#include "mesh.h"
#include "gl_common.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

GLenum glerr;
#define CHECK_GL_ERROR(txt)   \
if ((glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, glerr); \
throw std::runtime_error("GLR"); \
} \


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

  uint32_t norm_attr = -1;
  if (include_normals) {
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


  /*
   * Set up matrices
   */
  float fov = 20.0f;
  glm::mat4 project = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 20.0f);
  //glm::ortho(-1,1,-1,1);
  shader_->set_uniform("project", project);

  shader_->set_uniform("diff_light_int", 0.5f);
  shader_->set_uniform("diff_light_dir", glm::vec3(1, 1, 1));

  shader_->set_uniform("amb_light", 0.1f);

  shader_->set_uniform("spec_light_colour", glm::vec3(1, 1, 1));
  shader_->set_uniform("spec_light_alpha", 100000.0f);
}


Object::~Object() {
  destroy_buffers();
}

void Object::main_loop() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vao_);

  shader_->use();

  glm::mat4 view = glm::mat4(1);
  view = glm::translate(view, glm::vec3(0, 0, -10));
  view = glm::rotate(view, view_angle_, glm::vec3(0, 1, 0));

  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(-0.75, 0.0, 0.0));
  model = glm::rotate(model, head_1_angle_, glm::vec3(0, 1, 0));
  shader_->set_uniform("model_view", view * model);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);

  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(0.75, 0.0, 0.0));
  model = glm::rotate(model, -head_2_angle_, glm::vec3(0, 1, 0));
  shader_->set_uniform("model_view", view * model);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);

  head_1_angle_ += 0.01f;
  head_2_angle_ += 0.02f;
  view_angle_ += 0.004f;

}

void
Object::init_shader() {
  shader_ = Shader::from_files("lighting.vert", "lighting.frag");
}

void
Object::destroy_buffers() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteBuffers(1, &vao_);
}
