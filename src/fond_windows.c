#include "fond_internal.h"
#ifdef FOND_WIN

PFNGLTEXSTORAGE2DPROC glTexStorage2D = 0;
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
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = 0;
PFNGLCREATESHADERPROC glCreateShader = 0;
PFNGLSHADERSOURCEPROC glShaderSource = 0;
PFNGLCOMPILESHADERPROC glCompileShader = 0;
PFNGLGETSHADERIVPROC glGetShaderiv = 0;
PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
PFNGLATTACHSHADERPROC glAttachShader = 0;
PFNGLLINKPROGRAMPROC glLinkProgram = 0;
PFNGLGETPROGRAMIVPROC glGetProgramiv = 0;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = 0;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = 0;
PFNGLDELETESHADERPROC glDeleteShader = 0;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
PFNGLUSEPROGRAMPROC glUseProgram = 0;
PFNGLUNIFORM4FPROC glUniform4f = 0;
int loaded = 0;

void fond_load_glext(){
  if(!loaded){
    if((glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) wglGetProcAddress("glTexStorage2D"))==0)
      fprintf(stderr, "Failed to load glTexStorage2D\n");
    if((glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap"))==0)
      fprintf(stderr, "Failed to load glGenerateMipmap\n");
    if((glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) wglGetProcAddress("glGenVertexArrays"))==0)
      fprintf(stderr, "Failed to load glGenVertexArrays\n");
    if((glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers"))==0)
      fprintf(stderr, "Failed to load glGenBuffers\n");
    if((glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) wglGetProcAddress("glBindVertexArray"))==0)
      fprintf(stderr, "Failed to load glBindVertexArray\n");
    if((glBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer"))==0)
      fprintf(stderr, "Failed to load glBindBuffer\n");
    if((glBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData"))==0)
      fprintf(stderr, "Failed to load glBufferData\n");
    if((glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer"))==0)
      fprintf(stderr, "Failed to load glVertexAttribPointer\n");
    if((glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray"))==0)
      fprintf(stderr, "Failed to load glEnableVertexAttribArray\n");
    if((glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) wglGetProcAddress("glDeleteVertexArrays"))==0)
      fprintf(stderr, "Failed to load glDeleteVertexArrays\n");
    if((glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers"))==0)
      fprintf(stderr, "Failed to load glDeleteBuffers\n");
    if((glDeleteProgram = (PFNGLDELETEPROGRAMPROC) wglGetProcAddress("glDeleteProgram"))==0)
      fprintf(stderr, "Failed to load glDeleteProgram\n");
    if((glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers"))==0)
      fprintf(stderr, "Failed to load glDeleteFramebuffers\n");
    if((glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers"))==0)
      fprintf(stderr, "Failed to load glGenFramebuffers\n");
    if((glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer"))==0)
      fprintf(stderr, "Failed to load glBindFramebuffer\n");
    if((glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D"))==0)
      fprintf(stderr, "Failed to load glFramebufferTexture2D\n");
    if((glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatus"))==0)
      fprintf(stderr, "Failed to load glCheckFramebufferStatus\n");
    if((glCreateShader = (PFNGLCREATESHADERPROC) wglGetProcAddress("glCreateShader"))==0)
      fprintf(stderr, "Failed to load glCreateShader\n");
    if((glShaderSource = (PFNGLSHADERSOURCEPROC) wglGetProcAddress("glShaderSource"))==0)
      fprintf(stderr, "Failed to load glShaderSource\n");
    if((glCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader"))==0)
      fprintf(stderr, "Failed to load glCompileShader\n");
    if((glGetShaderiv = (PFNGLGETSHADERIVPROC) wglGetProcAddress("glGetShaderiv"))==0)
      fprintf(stderr, "Failed to load glGetShaderiv\n");
    if((glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram"))==0)
      fprintf(stderr, "Failed to load glCreateProgram\n");
    if((glAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader"))==0)
      fprintf(stderr, "Failed to load glAttachShader\n");
    if((glLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram"))==0)
      fprintf(stderr, "Failed to load glLinkProgram\n");
    if((glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv"))==0)
      fprintf(stderr, "Failed to load glGetProgramiv\n");
    if((glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog"))==0)
      fprintf(stderr, "Failed to load glGetShaderInfoLog\n");
    if((glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog"))==0)
      fprintf(stderr, "Failed to load glGetProgramInfoLog\n");
    if((glDeleteShader = (PFNGLDELETESHADERPROC) wglGetProcAddress("glDeleteShader"))==0)
      fprintf(stderr, "Failed to load glDeleteShader\n");
    if((glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation"))==0)
      fprintf(stderr, "Failed to load glGetUniformLocation\n");
    if((glUseProgram = (PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram"))==0)
      fprintf(stderr, "Failed to load glUseProgram\n");
    if((glUniform4f = (PFNGLUNIFORM4FPROC) wglGetProcAddress("glUniform4f"))==0)
      fprintf(stderr, "Failed to load glUniform4f\n");
    loaded = 1;
  }
}
#endif
