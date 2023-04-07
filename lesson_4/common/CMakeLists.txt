add_library(Lesson4 SHARED
        object.cc include/object.h
        )

target_include_directories(Lesson4
        PUBLIC
        include
        ${stb_SOURCE_DIR}
        )

target_link_libraries(Lesson4
        PUBLIC
        ${OpenGL_LIBRARY}
        GLHelpers
        )