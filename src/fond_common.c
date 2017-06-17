#include "fond_internal.h"
#include "utf8.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int errorcode = 0;

FOND_EXPORT int fond_decode_utf8(void *string, int32_t **_decoded, size_t *_size){
  if(utf8valid(string)){
    fond_err(FOND_UTF8_CONVERSION_ERROR);
    return 0;
  }
  
  size_t size = utf8len(string);
  int32_t *decoded = calloc(size+1, sizeof(uint32_t));
  if(!decoded){
    fond_err(FOND_OUT_OF_MEMORY);
    return 0;
  }
  
  for(int i=0; i<size; ++i){
    string = utf8codepoint(string, &decoded[i]);
  }

  *_decoded = decoded;
  if(_size) *_size = size;
  return 1;
}

FOND_EXPORT int fond_compute(struct fond_font *font, char *text, size_t *_n, GLuint *_vao){
  size_t size = 0;
  int32_t *codepoints = 0;
  
  if(!fond_decode_utf8((void *)text, &codepoints, &size)){
    return 0;
  }

  fond_compute_u(font, codepoints, size, _n, _vao);
  free(codepoints);
  return (errorcode == FOND_NO_ERROR);
}

FOND_EXPORT int fond_update(struct fond_font *font, char *text, size_t *_n, GLuint vbo, GLuint ebo){
  size_t size = 0;
  int32_t *codepoints = 0;
  
  if(!fond_decode_utf8((void *)text, &codepoints, &size)){
    return 0;
  }

  fond_update_u(font, codepoints, size, _n, vbo, ebo);
  free(codepoints);
  return (errorcode == FOND_NO_ERROR);
}

FOND_EXPORT int fond_compute_extent(struct fond_font *font, char *text, struct fond_extent *extent){
  size_t size = 0;
  int32_t *codepoints = 0;
  
  if(!fond_decode_utf8((void *)text, &codepoints, &size)){
    return 0;
  }

  fond_compute_extent_u(font, codepoints, size, extent);
  free(codepoints);
  return (errorcode == FOND_NO_ERROR);
}

FOND_EXPORT int fond_render(struct fond_buffer *buffer, char *text, float x, float y, float *color){
  size_t size = 0;
  int32_t *codepoints = 0;
  
  if(!fond_decode_utf8((void *)text, &codepoints, &size)){
    return 0;
  }

  fond_render_u(buffer, codepoints, size, x, y, color);
  free(codepoints);
  return (errorcode == FOND_NO_ERROR);
}

int fond_load_file(char *file, void **pointer){
  FILE *fd = fopen(file, "rb");
  if(!fd) return 0;

  struct stat finfo;
  if(fstat(fileno(fd), &finfo)) return 0;
  
  unsigned char *data = calloc(finfo.st_size, sizeof(char));
  if(!data) return 0;

  if(fread(data, sizeof(char), finfo.st_size, fd) < finfo.st_size){
    free(data);
    return 0;
  }

  *pointer = data;
  return 1;
}

void fond_err(int code){
  errorcode = code;
}

char *glErrorString(GLint err){
  switch(err){
  case GL_NO_ERROR:
    return "No error has been recorded.";
  case GL_INVALID_ENUM:
    return "An unacceptable value is specified for an enumerated argument.";
  case GL_INVALID_VALUE:
    return "A numeric argument is out of range.";
  case GL_INVALID_OPERATION:
    return "The specified operation is not allowed in the current state.";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "The framebuffer object is not complete.";
  case GL_OUT_OF_MEMORY:
    return "There is not enough memory left to execute the command.";
#ifndef FOND_MAC
  case GL_STACK_UNDERFLOW:
    return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
  case GL_STACK_OVERFLOW:
    return "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
#endif
  default:
    return "Unknown GL error value.";
  }
}

int fond_check_glerror(){
  GLint err = glGetError();
  if(err != GL_NO_ERROR){
    fprintf(stderr, "\nFond: OpenGL error %i: %s\n", err, glErrorString(err));
    return 0;
  }
  return 1;
}

int fond_check_shader(GLuint shader){
  GLint res = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
  if(res == GL_FALSE){
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar error[length];
    glGetShaderInfoLog(shader, length, &length, &error[0]);
    fprintf(stderr, "\nFond: GLSL error: %s\n", error);
    return 0;
  }
  return 1;
}

int fond_check_program(GLuint program){
  GLint res = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &res);
  if(res == GL_FALSE){
    GLint length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    GLchar error[length];
    glGetProgramInfoLog(program, length, &length, &error[0]);
    fprintf(stderr, "\nFond: GLSL error: %s\n", error);
    return 0;
  }
  return 1;
}

FOND_EXPORT enum fond_error fond_error(){
  return errorcode;
}

FOND_EXPORT char *fond_error_string(enum fond_error error){
  switch(error){
  case FOND_NO_ERROR:
    return "No error has occurred yet.";
  case FOND_FILE_LOAD_FAILED:
    return "Failed to load the font file. The file does not exist or is not accessible.";
  case FOND_OUT_OF_MEMORY:
    return "Allocation failure due to heap exhaustion.";
  case FOND_FONT_PACK_FAILED:
    return "Failed to pack the font. The atlas size was too small or the font file was invalid.";
  case FOND_FONT_INIT_FAILED:
    return "Failed to initialize the font. The font file was likely invalid.";
  case FOND_OPENGL_ERROR:
    return "An OpenGL error has been encountered.";
  case FOND_SIZE_EXCEEDED:
    return "Maximum size for font loading has been exceeded.";
  case FOND_NOT_LOADED:
    return "Cannot render as the font has not been loaded properly yet.";
  case FOND_UTF8_CONVERSION_ERROR:
    return "Failed to convert UTF8 string to UTF32. It is probably malformatted.";
  case FOND_UNLOADED_GLYPH:
    return "A glyph that was not loaded into the font was attempted to be drawn.";
  case FOND_NO_CHARACTERS_OR_CODEPOINTS:
    return "Font struct did not contain a character or a codepoints array.";
  default:
    return "Unknown error code.";
  }
}
