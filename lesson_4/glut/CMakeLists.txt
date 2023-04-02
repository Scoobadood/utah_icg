## Supress Apple OpenGL messages
IF (APPLE)
    FIND_LIBRARY(GLUT_LIBRARY GLUT)
ELSE()
    set(GLUT_LIBRARY freeglut)
ENDIF (APPLE)

add_executable(test_glut
        src/main.cc include/main.h
        )

target_include_directories(test_glut
        PRIVATE
        include)


target_link_libraries(test_glut
        PRIVATE
        ${GLUT_LIBRARY}
        Lesson4
        ${GLEW_LIBRARIES}
        )
