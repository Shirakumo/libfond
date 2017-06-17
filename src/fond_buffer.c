#include <stdio.h>
#include "fond_internal.h"
#include "shader/to_texture.vert.h"
#include "shader/to_texture.frag.h"

const GLchar *to_texture_vert_src = to_texture_vert;
const GLchar *to_texture_frag_src = to_texture_frag;

FOND_EXPORT void fond_free_buffer(struct fond_buffer *buffer){
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

FOND_EXPORT int fond_load_buffer(struct fond_buffer *buffer){
  GLuint vert = 0, frag = 0;
  
  if(buffer->width == 0)
    buffer->width = 512;
  if(buffer->height == 0)
    buffer->height = 64;
  
  glGenTextures(1, &buffer->texture);
  glBindTexture(GL_TEXTURE_2D, buffer->texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.65);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, buffer->width, buffer->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  if(!fond_check_glerror()){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_buffer_cleanup;
  }

  glGenFramebuffers(1, &buffer->framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if(!fond_check_glerror()
     || glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_buffer_cleanup;
  }

  vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &to_texture_vert_src, 0);
  glCompileShader(vert);
  if(!fond_check_shader(vert)){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_buffer_cleanup;
  }
  
  frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &to_texture_frag_src, 0);
  glCompileShader(frag);
  if(!fond_check_shader(frag)){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_buffer_cleanup;
  }

  buffer->program = glCreateProgram();
  glAttachShader(buffer->program, vert);
  glAttachShader(buffer->program, frag);
  glLinkProgram(buffer->program);
  if(!fond_check_program(buffer->program)){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_buffer_cleanup;
  }

  glGenVertexArrays(1, &buffer->vao);
  glGenBuffers(1, &buffer->vbo);
  glGenBuffers(1, &buffer->ebo);

  glBindVertexArray(buffer->vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (GLvoid*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if(!fond_check_glerror()){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_buffer_cleanup;
  }

  fond_err(FOND_NO_ERROR);
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

  if(buffer->ebo)
    glDeleteBuffers(1, &buffer->ebo);

  if(buffer->vbo)
    glDeleteBuffers(1, &buffer->vbo);

  if(buffer->vao)
    glDeleteVertexArrays(1, &buffer->vao);
  return 0;
}

FOND_EXPORT int fond_render_u(struct fond_buffer *buffer, int32_t *text, size_t size, float x, float y, float *color){
  size_t n;
  GLuint extent_u = 0, color_u = 0;
  
  if(!fond_update_u(buffer->font, text, size, &n, buffer->vbo, buffer->ebo)){
    return 0;
  }
  
  extent_u = glGetUniformLocation(buffer->program, "extent");
  color_u = glGetUniformLocation(buffer->program, "text_color");
  
  glBindFramebuffer(GL_FRAMEBUFFER, buffer->framebuffer);
  glViewport(0, 0, buffer->width, buffer->height);
  glUseProgram(buffer->program);
  glBindVertexArray(buffer->vao);
  glBindTexture(GL_TEXTURE_2D, buffer->font->atlas);
  glUniform4f(extent_u, x, y, buffer->width, buffer->height);
  if(color) glUniform4f(color_u, color[0], color[1], color[2], color[3]);
  {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
  }
  glBindTexture(GL_TEXTURE_2D, buffer->texture);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if(!fond_check_glerror()){
    fond_err(FOND_OPENGL_ERROR);
    return 0;
  }
  
  fond_err(FOND_NO_ERROR);
  return 1;
}
