#include "fond.h"
#include <stdio.h>
#if defined(WIN32) || defined(_WIN32)
#  define FOND_WIN
#  include <GL/gl.h>
#  include <GL/glext.h>
#  include "fond_windows.h"
#endif
#if !defined(WIN32) && !defined(_WIN32)
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#  include <GL/glext.h>
#endif

int errorcode;
void fond_err(int code);

int fond_load_file(char *file, void **content);
