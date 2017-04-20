#include "fond_common.h"
#include "fond.h"
#include "shader/to_texture.vert.h"
#include "shader/to_texture.frag.h"

void fond_free_buffer(struct fond_buffer *buffer){
  if(buffer->texture)
    glDeleteTextures(1, &buffer->texture);
  buffer->texture = 0;

  if(buffer->program)
    glDeleteProgram(buffer->program);
  buffer->program = 0;

  if(buffer->framebuffer)
    glDeleteFramebuffers(1, &buffer->framebuffer);
  buffer->framebuffer = 0;
}

int fond_load_buffer(struct fond_buffer *buffer){
  GLuint vert = 0, frag = 0;
  GLint res = 0;
  
  if(buffer->width == 0)
    buffer->width = 512;
  if(buffer->height == 0)
    buffer->height = 64;
  
  glGenTextures(1, &buffer->texture);
  glBindTexture(GL_TEXTURE_2D, buffer->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buffer->width, buffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  if(glGetError() != GL_NO_ERROR){
    errorcode = OPENGL_ERROR;
    goto fond_load_buffer_cleanup;
  }

  glGenFramebuffers(1, &buffer->framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer->texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if(glGetError() != GL_NO_ERROR
     || glCheckNamedFramebufferStatus(buffer->framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    errorcode = OPENGL_ERROR;
    goto fond_load_buffer_cleanup;
  }

  vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, (const GLchar *const*)&to_texture_vert, 0);
  glCompileShader(vert);
  glGetShaderiv(vert, GL_COMPILE_STATUS, &res);
  if(res == GL_FALSE){
    errorcode = OPENGL_ERROR;
    goto fond_load_buffer_cleanup;
  }
  
  frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, (const GLchar *const*)&to_texture_frag, 0);
  glCompileShader(frag);
  glGetShaderiv(frag, GL_COMPILE_STATUS, &res);
  if(res == GL_FALSE){
    errorcode = OPENGL_ERROR;
    goto fond_load_buffer_cleanup;
  }

  buffer->program = glCreateProgram();
  glAttachShader(buffer->program, vert);
  glAttachShader(buffer->program, frag);
  glLinkProgram(buffer->program);
  glGetProgramiv(buffer->program, GL_LINK_STATUS, &res);
  if(res == GL_FALSE){
    errorcode = OPENGL_ERROR;
    goto fond_load_buffer_cleanup;
  }
  
  glDeleteShader(vert);
  glDeleteShader(frag);

  return 1;

 fond_load_buffer_cleanup:
  if(buffer->texture)
    glDeleteTextures(1, &buffer->texture);
  buffer->texture = 0;

  if(buffer->framebuffer)
    glDeleteFramebuffers(1, &buffer->framebuffer);
  buffer->framebuffer = 0;
  
  if(vert)
    glDeleteShader(vert);
  
  if(frag)
    glDeleteShader(frag);

  if(buffer->program)
    glDeleteProgram(buffer->program);
  buffer->program = 0;
  return 0;
}

int fond_render(struct fond_buffer *buffer, char *text){
  float x, y;
  size_t n;
  GLuint vao = 0;

  if(!fond_compute(buffer->font, text, &n, &x, &y, &vao)){
    return 0;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);
  glUseProgram(buffer->program);
  glBindVertexArray(vao);
  glBindTexture(GL_TEXTURE_2D, buffer->font->texture);
  {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if(glGetError() != GL_NO_ERROR){
    glDeleteVertexArrays(1, &vao);
    errorcode = OPENGL_ERROR;
    return 0;
  }
  
  glDeleteVertexArrays(1, &vao);
  return 1;
}
