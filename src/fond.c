#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "fond_common.h"
#include "fond.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"
#include "utf8.h"

int errcode = 0;

void fond_free(struct fond_font *font){
  if(font->texture)
    glDeleteTextures(1, &font->texture);
  font->texture = 0;
  
  if(font->converted_codepoints && font->codepoints)
    free(font->codepoints);
  font->codepoints = 0;
  font->converted_codepoints = 0;
  
  if(font->data)
    free(font->data);
  font->data = 0;
}

int fond_pack_range(struct fond_font *font, stbtt_pack_range *range){
  unsigned int bytesize = 0, size = 0;

  if(font->codepoints){
    for(; font->codepoints[size]; ++size);
  }else{
    for(; font->characters[bytesize]; ++bytesize);
    font->codepoints = calloc(bytesize, sizeof(uint32_t));
    if(!font->codepoints){
      errcode = OUT_OF_MEMORY;
      goto fond_pack_range_cleanup;
    }
    
    if(!utf8_to_utf32(font->characters, font->codepoints, bytesize, size)){
      errcode = UTF8_CONVERSION_ERROR;
      goto fond_pack_range_cleanup;
    }
    
    font->converted_codepoints = 1;
  }
  
  font->data = calloc(size, sizeof(stbtt_packedchar));
  if(!font->data){
    errcode = OUT_OF_MEMORY;
    goto fond_pack_range_cleanup;
  }
  
  range->array_of_unicode_codepoints = font->codepoints;
  range->num_chars = size;
  range->chardata_for_range = font->data;
  return 1;

 fond_pack_range_cleanup:
  if(errcode = UTF8_CONVERSION_ERROR){
    if(font->codepoints)
      free(font->codepoints);
    font->codepoints = 0;
  }
  
  if(font->data)
    free(font->data);
  font->data = 0;
  
  return 0;
}

int fond_load_internal(struct fond_font *font, unsigned char *fontdata, stbtt_pack_range *range){
  stbtt_pack_context context = {0};
  unsigned char atlasdata[font->width*font->height];

  if(!atlasdata){
    errcode = OUT_OF_MEMORY;
    goto fond_load_internal_cleanup;
  }
  
  if(!stbtt_PackBegin(&context, atlasdata, font->width, font->height, 0, 1, 0)){
    errcode = FONT_PACK_FAILED;
    goto fond_load_internal_cleanup;
  }

  stbtt_PackSetOversampling(&context, font->oversample_h, font->oversample_v);

  if(!stbtt_PackFontRanges(&context, fontdata, font->index, &range, 1)){
    errcode = FONT_PACK_FAILED;
    goto fond_load_internal_cleanup;
  }

  stbtt_PackEnd(&context);
  free(fontdata);

  glGenTextures(1, &font->texture);
  glBindTexture(GL_TEXTURE_2D, font->texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->width, font->height, 0, GL_RED, GL_UNSIGNED_BYTE, atlasdata);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  if(glGetError() != GL_NO_ERROR){
    errcode = OPENGL_ERROR;
    goto fond_load_internal_cleanup;
  }
  
  return 1;

 fond_load_internal_cleanup:
  if(font->texture)
    glDeleteTextures(1, &font->texture);
  font->texture = 0;
  return 0;
}

int fond_load(struct fond_font *font){
  stbtt_pack_range range = {0};
  unsigned char *fontdata = fond_load_file(font->file);
  
  if(!fontdata){
    errcode = FILE_LOAD_FAILED;
    goto fond_load_cleanup;
  }

  if(!fond_pack_range(font, &range)){
    goto fond_load_cleanup;
  }

  if(!fond_load_internal(font, fontdata, &range)){
    goto fond_load_cleanup;
  }

  return 1;

 fond_load_cleanup:
  if(fontdata)
    free(fontdata);

  if(font->data)
    free(font->data);
  font->data = 0;

  return 0;
}

int fond_load_fit(struct fond_font *font, unsigned int max_size){
  stbtt_pack_range range = {0};
  unsigned char *fontdata = fond_load_file(font->file);

  if(!fontdata){
    errcode = FILE_LOAD_FAILED;
    goto fond_load_fit_cleanup;
  }

  if(!fond_pack_range(font, &range)){
    goto fond_load_fit_cleanup;
  }
  
  if(font->width == 0) font->width = 128;
  if(font->height == 0) font->height = 128;
  
  while(font->width < max_size){
    if(fond_load_internal(font, fontdata, &range))
      return 1;

    switch(errcode){
    case FILE_LOAD_FAILED:
    case OUT_OF_MEMORY:
      goto fond_load_fit_cleanup;
    case FONT_PACK_FAILED:
      font->width *= 2;
      font->height *= 2;
    }
  }
  
  errcode = SIZE_EXCEEDED;

 fond_load_fit_cleanup:
  if(fontdata)
    free(fontdata);

  if(font->data)
    free(font->data);
  font->data = 0;

  return 0;
}

unsigned int fond_codepoint_index(struct fond_font *font, uint32_t glyph){
  for(unsigned int i=0; font->codepoints[i]; ++i){
    if(font->codepoints[i] == glyph)
      return i;
  }
  return -1;
}

int fond_compute_glyph(struct fond_font *font, uint32_t glyph, float *x, float *y, int i, GLfloat  *vert, GLfloat *tex, GLuint *ind){
  stbtt_aligned_quad quad = {0};
  unsigned int index = fond_codepoint_index(font, glyph);
  if(index < 0){
    errcode = UNLOADED_GLYPH;
    return 1;
  }
  
  stbtt_GetPackedQuad((stbtt_packedchar *)font->data, font->width, font->height, index, x, y, &quad, 1);

  int vi = i*4*3;
  vert[vi++] = quad.x0; vert[vi++] = -quad.y0; vert[vi++] = 0;
  vert[vi++] = quad.x0; vert[vi++] = -quad.y1; vert[vi++] = 0;
  vert[vi++] = quad.x1; vert[vi++] = -quad.y1; vert[vi++] = 0;
  vert[vi++] = quad.x1; vert[vi++] = -quad.y0; vert[vi++] = 0;

  int ti = i*4*2;
  tex[ti++] = quad.s0; tex[ti++] = quad.t1;
  tex[ti++] = quad.s0; tex[ti++] = quad.t0;
  tex[ti++] = quad.s1; tex[ti++] = quad.t0;
  tex[ti++] = quad.s1; tex[ti++] = quad.t1;
  
  int ii = i*2*3;
  ind[ii++] = i; ind[ii++] = i+1; ind[ii++] = i+2;
  ind[ii++] = i; ind[ii++] = i+2; ind[ii++] = i+3;

  return 0;
}


int fond_compute(struct fond_font *font, char *text, unsigned int *_n, float *_x, float *_y, GLuint *_vao){
  if(!font->data){
    errcode = NOT_LOADED;
    return 0;
  }

  unsigned int bytesize = 0, size = 0;
  for(; text[bytesize]; ++bytesize);
  uint32_t codepoints[bytesize];
  utf8_to_utf32(text, codepoints, bytesize, size);
  
  GLfloat vert[4*3*size]; // Quad is 4 vertices with 3 floats
  GLfloat tex[4*2*size];  // Quad is 4 texcoords with 2 floats
  GLuint ind[2*3*size];   // Quad is 2 triangles with 3 points

  float x = 0, y = 0;
  unsigned int i = 0;
  for(; i<size; ++i){
    fond_compute_glyph(font, codepoints[i], &x, &y, i, vert, tex, ind);
  }

  GLuint vao[1];
  glGenVertexArrays(1, vao);
  GLuint vbo[3];
  glGenBuffers(3, vbo);

  glBindVertexArray(vao[0]);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*3*size, vert, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*2*size, tex, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6*size, ind, GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(3, vbo);

  glBindVertexArray(0);

  if(glGetError() != GL_NO_ERROR){
    glDeleteVertexArrays(1, vao);
    errcode = OPENGL_ERROR;
    return 0;
  }

  *_n = i*2;
  *_x = x;
  *_y = y;
  *_vao = vao[0];
  return 1;
}
