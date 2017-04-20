#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STBTT_STATIC
#define STBRP_STATIC

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "fond_common.h"
#include "fond.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"

void fond_free(struct fond_font *font){
  if(font->atlas)
    glDeleteTextures(1, &font->atlas);
  font->atlas = 0;
  
  if(font->converted_codepoints && font->codepoints)
    free(font->codepoints);
  font->codepoints = 0;
  font->converted_codepoints = 0;
  
  if(font->data)
    free(font->data);
  font->data = 0;
}

int fond_pack_range(struct fond_font *font, stbtt_pack_range *range){
  size_t bytesize = 0, size = 0;

  if(font->codepoints){
    for(; font->codepoints[size]; ++size);
  }else if(font->characters){
    if(!fond_decode_utf8((void *)font->characters, &font->codepoints, 0)){
      return 0;
    }
    
    font->converted_codepoints = 1;
  }else{
    errorcode = NO_CHARACTERS_OR_CODEPOINTS;
    return 0;
  }
  
  font->data = calloc(size, sizeof(stbtt_packedchar));
  if(!font->data){
    if(font->data)
      free(font->data);
    font->data = 0;
    errorcode = OUT_OF_MEMORY;
    return 0;
  }
  
  range->font_size = font->size;
  range->array_of_unicode_codepoints = (int *)font->codepoints;
  range->num_chars = size;
  range->chardata_for_range = font->data;
  return 1;
}

int fond_load_internal(struct fond_font *font, unsigned char *fontdata, stbtt_pack_range *range){
  stbtt_pack_context context = {0};
  unsigned char atlasdata[font->width*font->height];
  
  if(!stbtt_PackBegin(&context, atlasdata, font->width, font->height, 0, 1, 0)){
    errorcode = FONT_PACK_FAILED;
    goto fond_load_internal_cleanup;
  }

  if(0 < font->oversample_h && 0 < font->oversample_v)
    stbtt_PackSetOversampling(&context, font->oversample_h, font->oversample_v);

  if(!stbtt_PackFontRanges(&context, fontdata, font->index, range, 1)){
    errorcode = FONT_PACK_FAILED;
    goto fond_load_internal_cleanup;
  }

  stbtt_PackEnd(&context);
  free(fontdata);

  glGenTextures(1, &font->atlas);
  glBindTexture(GL_TEXTURE_2D, font->atlas);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->width, font->height, 0, GL_RED, GL_UNSIGNED_BYTE, atlasdata);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  if(glGetError() != GL_NO_ERROR){
    errorcode = OPENGL_ERROR;
    goto fond_load_internal_cleanup;
  }
  
  return 1;

 fond_load_internal_cleanup:
  if(font->atlas)
    glDeleteTextures(1, &font->atlas);
  font->atlas = 0;
  return 0;
}

int fond_load(struct fond_font *font){
  stbtt_pack_range range = {0};
  unsigned char *fontdata = fond_load_file(font->file);
  
  if(!fontdata){
    errorcode = FILE_LOAD_FAILED;
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
    errorcode = FILE_LOAD_FAILED;
    goto fond_load_fit_cleanup;
  }

  if(!fond_pack_range(font, &range)){
    goto fond_load_fit_cleanup;
  }
  
  if(font->width == 0) font->width = 64;
  if(font->height == 0) font->height = 64;
  
  while(font->width < max_size){
    if(fond_load_internal(font, fontdata, &range))
      return 1;

    switch(errorcode){
    case FILE_LOAD_FAILED:
    case OUT_OF_MEMORY:
      goto fond_load_fit_cleanup;
    case FONT_PACK_FAILED:
      font->width *= 2;
      font->height *= 2;
    }
  }
  
  errorcode = SIZE_EXCEEDED;

 fond_load_fit_cleanup:
  if(fontdata)
    free(fontdata);

  if(font->data)
    free(font->data);
  font->data = 0;

  return 0;
}

int fond_codepoint_index(struct fond_font *font, uint32_t glyph){
  for(size_t i=0; font->codepoints[i]; ++i){
    if(font->codepoints[i] == glyph)
      return i;
  }
  return -1;
}

int fond_compute_glyph(struct fond_font *font, uint32_t glyph, float *x, float *y, size_t i, GLfloat  *vert, GLuint *ind){
  stbtt_aligned_quad quad = {0};
  int index = fond_codepoint_index(font, glyph);
  if(index < 0){
    errorcode = UNLOADED_GLYPH;
    return 1;
  }
  
  stbtt_GetPackedQuad((stbtt_packedchar *)font->data, font->width, font->height, index, x, y, &quad, 1);

  int vi = 4*(3+2)*i;
  vert[vi++] = quad.x0; vert[vi++] = -quad.y0; vert[vi++] = 0; vert[vi++] = quad.s0; vert[vi++] = quad.t1;
  vert[vi++] = quad.x0; vert[vi++] = -quad.y1; vert[vi++] = 0; vert[vi++] = quad.s0; vert[vi++] = quad.t0;
  vert[vi++] = quad.x1; vert[vi++] = -quad.y1; vert[vi++] = 0; vert[vi++] = quad.s1; vert[vi++] = quad.t0;
  vert[vi++] = quad.x1; vert[vi++] = -quad.y0; vert[vi++] = 0; vert[vi++] = quad.s1; vert[vi++] = quad.t1;
  
  int ii = 2*3*i;
  ind[ii++] = i; ind[ii++] = i+1; ind[ii++] = i+2;
  ind[ii++] = i; ind[ii++] = i+2; ind[ii++] = i+3;

  return 0;
}


int fond_compute(struct fond_font *font, char *text, size_t *_n, float *_x, float *_y, GLuint *_vao){
  if(!font->data){
    errorcode = NOT_LOADED;
    return 0;
  }

  size_t size = 0;
  int32_t *codepoints;
  if(!fond_decode_utf8((void *)text, &codepoints, &size)){
    return 0;
  }
  
  GLfloat vert[4*(3+2)*size];
  GLuint ind[2*3*size];

  float x = 0, y = 0;
  size_t i = 0;
  for(; i<size; ++i){
    fond_compute_glyph(font, codepoints[i], &x, &y, i, vert, ind);
  }

  GLuint vao[1], vbo[2];
  glGenVertexArrays(1, vao);
  glGenBuffers(2, vbo);

  glBindVertexArray(vao[0]);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*(3+2)*size, vert, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*(3+2), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*(3+2), (GLvoid*)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6*size, ind, GL_STATIC_DRAW);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(2, vbo);

  if(glGetError() != GL_NO_ERROR){
    free(codepoints);
    glDeleteVertexArrays(1, vao);
    errorcode = OPENGL_ERROR;
    return 0;
  }

  *_n = i*2*3;
  *_x = x;
  *_y = y;
  *_vao = vao[0];
  return 1;
}
