find_package(OpenGL REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(GLHelpers
        SHARED
        src/shader.cc include/shader.h
        src/mesh.cc include/mesh.h
        src/string_utils.cc include/string_utils.h
        )

target_include_directories(GLHelpers
        PRIVATE
        ${spdlog_SOURCE_DIR}/include
        PUBLIC
        include
        )

target_link_libraries(GLHelpers
        PUBLIC
        ${OPENGL_LIBRARIES}
        glm
        )

add_executable(test_obj_loader
        tests/test_obj_parser.cc
        )


target_link_libraries(test_obj_loader
        PRIVATE
        GLHelpers
        gtest
        ${GLEW_LIBRARIES}
        )
