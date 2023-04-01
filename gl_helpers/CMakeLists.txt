find_package(OpenGL REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

add_library(GLHelpers
        SHARED
        src/shader.cc include/shader.h
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
