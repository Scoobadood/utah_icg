## Supress Apple OpenGL messages
IF (APPLE)
    FIND_LIBRARY(GLUT_LIBRARY GLUT)
ENDIF (APPLE)

add_executable(test_glut
        src/main.cc
        )


target_link_libraries(test_glut
        PRIVATE
        ${GLUT_LIBRARY}
        Lesson4
        )
