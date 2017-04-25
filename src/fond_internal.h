#include "fond.h"
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32)
#  define FOND_WIN
#  include <GL/gl.h>
#  include <GL/glext.h>
#  include "fond_windows.h"
#  include <GL/glu.h>
#endif
#if defined(__APPLE__)
#  define GL_GLEXT_PROTOTYPES
#  include <OpenGL/gl.h>
#  include <OpenGL/glext.h>
#  include <OpenGL/glu.h>
#endif
#if defined(__linux__)
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glext.h>
#  include <GL/glu.h>
#endif

int errorcode;
void fond_err(int code);
int fond_check_glerror();
int fond_check_shader(GLuint shader);
int fond_check_program(GLuint program);

int fond_load_file(char *file, void **content);
