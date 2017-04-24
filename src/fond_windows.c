#include "fond.h"
#ifdef FOND_WIN

PFNGLGENERATEMIPMAPPROC glGenerateMipmap = 0;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = 0;
PFNGLGENBUFFERSPROC glGenBuffers = 0;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = 0;
PFNGLBINDBUFFERPROC glBindBuffer = 0;
PFNGLBUFFERDATAPROC glBufferData = 0;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = 0;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = 0;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = 0;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = 0;
PFNGLDELETEPROGRAMPROC glDeleteProgram = 0;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = 0;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = 0;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = 0;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = 0;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus = 0;
PFNGLCREATESHADERPROC glCreateShader = 0;
PFNGLSHADERSOURCEPROC glShaderSource = 0;
PFNGLCOMPILESHADERPROC glCompileShader = 0;
PFNGLGETSHADERIVPROC glGetShaderiv = 0;
PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
PFNGLATTACHSHADERPROC glAttachShader = 0;
PFNGLLINKPROGRAMPROC glLinkProgram = 0;
PFNGLGETPROGRAMIVPROC glGetProgramiv = 0;
PFNGLDELETESHADERPROC glDeleteShader = 0;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
PFNGLUSEPROGRAMPROC glUseProgram = 0;
PFNGLUNIFORM4FPROC glUniform4f = 0;

void fond_startup(){
  glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) wglGetProcAddress("glGenVertexArrays");
  glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) wglGetProcAddress("glBindVertexArray");
  glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) wglGetProcAddress("glDeleteVertexArrays");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram");
  glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers");
  glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
  glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
  glCheckNamedFramebufferStatus = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckNamedFramebufferStatus");
  glCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
  glGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
  glAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
  glDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
  glUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
  glUniform4f = (PFNGLUNIFORM4FPROC) wglGetProcAddress("glUniform4f");
}
#endif
