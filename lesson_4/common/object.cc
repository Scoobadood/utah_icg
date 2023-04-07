#include "object.h"
#include "spdlog/spdlog-inl.h"
#include "mesh.h"
#include "gl_common.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"


GLenum glerr;
#define CHECK_GL_ERROR(txt)   \
if ((glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, glerr); \
throw std::runtime_error("GLR"); \
} \


int32_t load_texture(const std::string &texture_filename) {
  uint32_t tx_id = 0;

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &tx_id);
  glBindTexture(GL_TEXTURE_2D, tx_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  int32_t width, height, channels;
  uint8_t *image_data = stbi_load(texture_filename.c_str(), &width, &height, &channels, 3);

  // Flip image
  for (auto row = 0; row < height/2; ++row) {
    for (auto col = 0; col < width; ++col) {
      auto from_idx = (row * width + col) * 3;
      auto to_idx = ((height - row - 1) * width + col) * 3;
      for (auto c = 0; c < 3; ++c) {
        uint8_t t = image_data[from_idx + c];
        image_data[from_idx + c] = image_data[to_idx + c];
        image_data[to_idx + c] = t;
      }
    }
  }
  // load file to texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, image_data);
  stbi_image_free(image_data);
  return tx_id;
}

Object::Object(const std::string &file_name,
               bool include_normals,
               bool include_tex_coords) {
  init_shader();
  if (!shader_->is_good()) {
    return;
  }

  textured_shader_ = Shader::from_files("lighting.vert", "lighting_tex.frag");
  if (!textured_shader_->is_good()) {
    return;
  }

  texture_id_ = load_texture("african_head_diffuse.tga");

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
  if (include_tex_coords) {
    tx_attr = shader_->get_attribute_location("tex_coords");
    if (tx_attr == -1) {
      spdlog::warn("Missing attribute location tex_coords. Disabling textures.");
      include_tex_coords = false;
    }
  }

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
  shader_->set_uniform("light_dir", glm::vec3(0, -1, -0.5));

  shader_->set_uniform("spot_light_pos", glm::vec3(0, 0, -1));
  shader_->set_uniform("spot_light_dir", glm::vec3(0, 0, 1));
  shader_->set_uniform("spot_light_colour", glm::vec3(1, 1, 1));
  shader_->set_uniform("spot_light_angle", cosf(8.0f * M_PI / 180.0f));

  textured_shader_->use();
  textured_shader_->set_uniform("project", project);
  textured_shader_->set_uniform("light_dir", glm::vec3(0, -1, -0.5));
  textured_shader_->set_uniform("spot_light_pos", glm::vec3(0, 0, -1));
  textured_shader_->set_uniform("spot_light_dir", glm::vec3(0, 0, 1));
  textured_shader_->set_uniform("spot_light_colour", glm::vec3(1, 1, 1));
  textured_shader_->set_uniform("spot_light_angle", cosf(8.0f * M_PI / 180.0f));
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
  shader_->set_uniform("light_int", big_light_on_ ? 1.0f : 0.1f);
  shader_->set_uniform("spot_light_int", spot_on_ ? 1.f : 0.0f);


  glm::mat4 view = glm::mat4(1);
  view = glm::translate(view, glm::vec3(0, 0, -10));
  view = glm::rotate(view, view_angle_, glm::vec3(0, 1, 0));

  // Head 1
  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
  model = glm::rotate(model, head_1_angle_, glm::vec3(0, 1, 0));
  shader_->set_uniform("model_view", view * model);
  shader_->set_uniform("kd", 0.8f);
  shader_->set_uniform("ks", 0.5f);
  shader_->set_uniform("ka", 0.1f);
  shader_->set_uniform("colour", glm::vec3(0.7, 0., 0.));
  shader_->set_uniform("alpha", 100.0f);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);

  // Head 2
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(-1, 0.0, 1));
  model = glm::rotate(model, -head_2_angle_, glm::vec3(0, 1, 0));
  shader_->set_uniform("model_view", view * model);
  shader_->set_uniform("kd", 0.8f);
  shader_->set_uniform("ks", 0.5f);
  shader_->set_uniform("ka", 0.1f);
  shader_->set_uniform("colour", glm::vec3(0.0, 0.7, 0.));
  shader_->set_uniform("alpha", 1000.0f);

  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);

  // Head 3
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(1, 0.0, 1));
  model = glm::rotate(model, head_3_angle_, glm::vec3(0, 1, 0));
  shader_->set_uniform("model_view", view * model);
  shader_->set_uniform("kd", 0.8f);
  shader_->set_uniform("ks", 0.5f);
  shader_->set_uniform("ka", 0.1f);
  shader_->set_uniform("colour", glm::vec3(0.0, 0.0, 0.7));
  shader_->set_uniform("alpha", 10.0f);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);


  // Head 4
  model = glm::mat4(1.0);
  model = glm::translate(model, glm::vec3(1, 0.0, -1));
  model = glm::rotate(model, head_1_angle_, glm::vec3(0, 1, 0));
  textured_shader_->use();
  textured_shader_->set_uniform("light_int", big_light_on_ ? 1.0f : 0.1f);
  textured_shader_->set_uniform("spot_light_int", spot_on_ ? 1.f : 0.0f);

  textured_shader_->set_uniform("model_view", view * model);
  textured_shader_->set_uniform("kd", 0.8f);
  textured_shader_->set_uniform("ks", 0.f);
  textured_shader_->set_uniform("ka", 0.1f);
  textured_shader_->set_uniform("alpha", 10.f);
  textured_shader_->set_uniform("head_texture", 0);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);

  head_1_angle_ += 0.01f;
  head_2_angle_ += 0.01f;
  head_3_angle_ += 0.02f;
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
  glDeleteTextures(1, &texture_id_);
}
