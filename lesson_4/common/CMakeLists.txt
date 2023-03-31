add_library(Lesson4 SHARED
        main_loop.cc include/main_loop.h
        )

target_include_directories(Lesson4
        PUBLIC
        include
        )

target_link_libraries(Lesson4
        PUBLIC
        ${OpenGL_LIBRARY}
        )