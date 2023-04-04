#ifndef UTAH_ICG_MESH_H
#define UTAH_ICG_MESH_H

#ifdef __APPLE__

#include "OpenGL/gl3.h"

#else
#include "GL/glew.h"
#include "GL/glext.h"
#include "GL/gl.h"
#endif

#include <string>

bool load_obj(const std::string &obj_file_name,
              uint32_t &vao,
              uint32_t &vbo,
              uint32_t &ebo,
              uint32_t pos_attr,
              bool include_normals = false,
              uint32_t norm_attr = 0,
              bool include_textures = false,
              uint32_t tx_attr = 0
);

#endif //UTAH_ICG_MESH_H
