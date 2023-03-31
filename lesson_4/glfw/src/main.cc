/*
 * Lecture 4 tutorial.
 * Generate a window and render a scene using OpenGL
 * - Open a window.
 * - Render a teapot.
 * - Respond to input.
 */

#include "spdlog/spdlog-inl.h"
#include <GLFW/glfw3.h>
#include "main_loop.h"


void special_keyboard_handler(GLFWwindow* window, int key, int scancode, int action, int mods){}

void mouse_handler(GLFWwindow* window, int button, int action, int mods){}

void drag_handler(int32_t x,
                  int32_t y){
}

void mouse_motion_handler(int32_t x,
                          int32_t y){
}

void framebuffer_size_callback(GLFWwindow* window,int32_t width, int32_t height){}
void window_reshape_handler(GLFWwindow* window, int32_t width, int32_t height){}


void idle_handler() {
}

int main( int argc, char * argv[]) {
  if( !glfwInit()) return EXIT_FAILURE;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

  // Window creation
  auto window = glfwCreateWindow(800, 600,
                                 "My Window",
                                 nullptr, nullptr);
  if( !window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Create window with graphics context
  glfwMakeContextCurrent(window);


  glfwSwapInterval(1); // Enable vsync

  glfwSetWindowSizeCallback(window, window_reshape_handler);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, special_keyboard_handler);

  while( !glfwWindowShouldClose(window)) {
    main_loop();

    idle_handler();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;

}