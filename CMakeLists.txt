cmake_minimum_required(VERSION 3.22)

project(utah_icg)

set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")


# Write targets to bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#
# Deps
#
#
include(FetchContent)
# Spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
# Google test
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main
)
# GLM
option(GLM_TEST_ENABLE "Disable" OFF)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8
        GIT_PROGRESS TRUE
)
# FreeGLUT
add_definitions(-DGL_SILENCE_DEPRECATION)
FetchContent_Declare(
        freeglut
        GIT_REPOSITORY https://github.com/FreeGLUTProject/freeglut
        GIT_TAG v3.4.0
        GIT_PROGRESS TRUE
)
#GLEW
find_package(GLEW REQUIRED)
FetchContent_MakeAvailable(googletest spdlog glm freeglut)

message(ERROR ${GLEW_INCLUDE_DIRS})
message(ERROR ${GLEW_LIBRARIES})


include_directories(${spdlog_SOURCE_DIR}/include)


## Supress Apple OpenGL messages
IF (APPLE)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    add_definitions(-DGL_SILENCE_DEPRECATION)
ENDIF (APPLE)

add_subdirectory(gl_helpers)
add_subdirectory(lesson_4)

