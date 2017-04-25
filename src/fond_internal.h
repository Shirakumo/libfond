#include "fond.h"
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32)
#  define FOND_WIN
#  include <GL/gl.h>
#  include <GL/glext.h>
#  include "fond_windows.h"
#endif
#if defined(__APPLE__)
#  define FOND_MAC
#  define GL_GLEXT_PROTOTYPES
#  include <OpenGL/gl3.h>
#endif
#if defined(__linux__)
#  define FOND_LIN
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glext.h>
#endif

int errorcode;
void fond_err(int code);
int fond_check_glerror();
int fond_check_shader(GLuint shader);
int fond_check_program(GLuint program);

int fond_load_file(char *file, void **content);
