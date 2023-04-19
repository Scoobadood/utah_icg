/*
 * Lecture 4 tutorial.
 * Generate a window and render a scene using OpenGL
 * - Open a window.
 * - Render a teapot.
 * - Respond to input.
 */

#include "object.h"

#include "main.h"
#include "spdlog/spdlog-inl.h"

struct State {
  std::shared_ptr<Object> obj;
} g_state;

void display_handler() {
  g_state.obj->main_loop();
  glutSwapBuffers();
}

void keyboard_handler(uint8_t key, int32_t x, int32_t y) {
  switch (key) {
    case '1':
      g_state.obj->toggle_spot(0);
      break;
    case '2':
      g_state.obj->toggle_spot(1);
      break;
    case '3':
      g_state.obj->toggle_spot(2);
      break;
    case '4':
      g_state.obj->toggle_spot(3);
      break;
    case 'm':
      g_state.obj->big_light_on(false);
      break;
    case 'M':
      g_state.obj->big_light_on(true);
      break;

    case '-':
      g_state.obj->set_view_dist(g_state.obj->view_dist() - 5.0f);
      break;

    case '+':
      g_state.obj->set_view_dist(g_state.obj->view_dist() + 5.0f);
      break;

    case 'Z':
      g_state.obj->set_view_dir(Object::POS_Z);
      break;
    case 'z':
      g_state.obj->set_view_dir(Object::NEG_Z);
      break;
    case 'X':
      g_state.obj->set_view_dir(Object::POS_X);
      break;
    case 'x':
      g_state.obj->set_view_dir(Object::NEG_X);
      break;
    case 'Y':
      g_state.obj->set_view_dir(Object::POS_Y);
      break;
    case 'y':
      g_state.obj->set_view_dir(Object::NEG_Y);
      break;
  }

  glutPostRedisplay();
}

void special_keys_handler(int32_t key, int32_t x, int32_t y) {
}

void mouse_handler(int32_t button,
                   int32_t state,
                   int32_t x,
                   int32_t y) {
}

void drag_handler(int32_t x,
                  int32_t y) {
}

void mouse_motion_handler(int32_t x,
                          int32_t y) {
}

void window_reshape_handler(int32_t x, int32_t y) {
  glViewport(0, 0, x, y);
}

void idle_handler() {
  // Handle animations

  // Tell GLUT to redisplay
  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);

  // Set OGL version on non-Apple platforms
#ifndef __APPLE__
  glutInitContextVersion( 4, 1 );
  glutInitContextProfile( GLUT_CORE_PROFILE );
#endif


  /*
   * Note use of GLUT_3_2_CORE_PROFILE which is what Macos requires
   * to get a profile higher than 3.0. this supports 4.1 OK
   * gltInitcontextVersion is not available in the version of GLUT
   * supported on Macs, it's a FreeGLUT extension.
   */
  glutInitDisplayMode(GLUT_DOUBLE |
                      GLUT_RGBA |
                      GLUT_DEPTH |
                      GLUT_3_2_CORE_PROFILE
  );


  // Window creation
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("My window");

#ifndef __APPLE__
  // Need context before we do this.
  GLenum err = glewInit();
  if( GLEW_OK != err) {
      spdlog::critical("Error: {}", (const char *)glewGetErrorString(err));
  }
#endif

  // Callbacks - AFTER Window Creation!
  glutKeyboardFunc(keyboard_handler);
  glutSpecialFunc(special_keys_handler);
  glutMouseFunc(mouse_handler);
  glutMotionFunc(drag_handler);
  glutPassiveMotionFunc(mouse_motion_handler);
  glutReshapeFunc(window_reshape_handler);
  glutIdleFunc(idle_handler);

  g_state.obj = std::make_shared<Object>(argv[1], true, true);

  glutDisplayFunc(display_handler);

  glutMainLoop();
  return 0;

}