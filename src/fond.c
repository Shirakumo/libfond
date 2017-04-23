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
  
  if(font->chardata)
    free(font->chardata);
  font->chardata = 0;

  if(font->fontdata)
    free(font->fontdata);
  font->fontdata = 0;
}

int fond_pack_range(struct fond_font *font, stbtt_pack_range *range){
  size_t size = 0;

  if(font->codepoints){
    for(; font->codepoints[size]; ++size);
  }else if(font->characters){
    if(!fond_decode_utf8((void *)font->characters, &font->codepoints, &size)){
      return 0;
    }
    
    font->converted_codepoints = 1;
  }else{
    fond_err(NO_CHARACTERS_OR_CODEPOINTS);
    return 0;
  }
  
  font->chardata = calloc(size, sizeof(stbtt_packedchar));
  if(!font->chardata){
    if(font->chardata)
      free(font->chardata);
    font->chardata = 0;
    fond_err(OUT_OF_MEMORY);
    return 0;
  }
  
  fond_err(NO_ERROR);
  range->font_size = font->size;
  range->array_of_unicode_codepoints = (int *)font->codepoints;
  range->num_chars = size;
  range->chardata_for_range = font->chardata;
  return 1;
}

int fond_load_internal(struct fond_font *font, stbtt_pack_range *range){
  stbtt_pack_context context = {0};
  unsigned char *atlasdata = calloc(font->width*font->height, sizeof(char));

  if(!atlasdata){
    fond_err(OUT_OF_MEMORY);
    goto fond_load_internal_cleanup;
  }

  font->fontinfo = calloc(1, sizeof(stbtt_fontinfo));

  if(!stbtt_InitFont(font->fontinfo, font->fontdata, stbtt_GetFontOffsetForIndex(font->fontdata, font->index))){
    fond_err(FONT_INIT_FAILED);
    goto fond_load_internal_cleanup;
  }
  
  if(!stbtt_PackBegin(&context, atlasdata, font->width, font->height, 0, 1, 0)){
    fond_err(FONT_PACK_FAILED);
    goto fond_load_internal_cleanup;
  }

  if(0 < font->oversample)
    stbtt_PackSetOversampling(&context, font->oversample, font->oversample);

  if(!stbtt_PackFontRanges(&context, font->fontdata, font->index, range, 1)){
    fond_err(FONT_PACK_FAILED);
    goto fond_load_internal_cleanup;
  }

  stbtt_PackEnd(&context);

  glGenTextures(1, &font->atlas);
  glBindTexture(GL_TEXTURE_2D, font->atlas);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.65);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->width, font->height, 0, GL_RED, GL_UNSIGNED_BYTE, atlasdata);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  if(glGetError() != GL_NO_ERROR){
    fond_err(OPENGL_ERROR);
    goto fond_load_internal_cleanup;
  }
  
  fond_err(NO_ERROR);
  return 1;

 fond_load_internal_cleanup:
  if(atlasdata)
    free(atlasdata);
  
  if(font->fontinfo)
    free(font->fontinfo);
  font->fontinfo = 0;
  
  if(font->atlas)
    glDeleteTextures(1, &font->atlas);
  font->atlas = 0;
  return 0;
}

int fond_load(struct fond_font *font){
  stbtt_pack_range range = {0};
  
  if(!fond_load_file(font->file, &font->fontdata)){
    fond_err(FILE_LOAD_FAILED);
    goto fond_load_cleanup;
  }

  if(!fond_pack_range(font, &range)){
    goto fond_load_cleanup;
  }

  if(!fond_load_internal(font, &range)){
    goto fond_load_cleanup;
  }

  return 1;

 fond_load_cleanup:
  if(font->chardata)
    free(font->chardata);
  font->chardata = 0;

  if(font->fontdata)
    free(font->fontdata);
  font->fontdata = 0;

  return 0;
}

int fond_load_fit(struct fond_font *font, unsigned int max_size){
  stbtt_pack_range range = {0};

  if(!fond_load_file(font->file, &font->fontdata)){
    fond_err(FILE_LOAD_FAILED);
    goto fond_load_fit_cleanup;
  }

  if(!fond_pack_range(font, &range)){
    goto fond_load_fit_cleanup;
  }
  
  if(font->width == 0) font->width = 64;
  if(font->height == 0) font->height = 64;
  
  while(font->width <= max_size){
    if(fond_load_internal(font, &range)){
      return 1;
    }

    switch(errorcode){
    case FONT_INIT_FAILED:
    case OPENGL_ERROR:
    case OUT_OF_MEMORY:
      goto fond_load_fit_cleanup;
    case FONT_PACK_FAILED:
      font->width *= 2;
      font->height *= 2;
    }
  }
  
  fond_err(SIZE_EXCEEDED);

 fond_load_fit_cleanup:
  if(font->chardata)
    free(font->chardata);
  font->chardata = 0;

  if(font->fontdata)
    free(font->fontdata);
  font->fontdata = 0;

  return 0;
}

int fond_codepoint_index(struct fond_font *font, uint32_t glyph){
  for(size_t i=0; font->codepoints[i]; ++i){
    if(font->codepoints[i] == glyph)
      return i;
  }
  return -1;
}

int fond_compute_u(struct fond_font *font, int32_t *text, size_t size, size_t *_n, GLuint *_vao){
  GLfloat *vert = 0;
  GLuint *ind = 0;
  GLuint vao = 0, vbo = 0, ebo = 0;
  float x = 0, y = 0;
  
  if(!font->chardata){
    fond_err(NOT_LOADED);
    goto fond_compute_cleanup;
  }

  vert = calloc(4*4*size, sizeof(GLfloat));
  ind = calloc(2*3*size, sizeof(GLuint));
  if(!vert || !ind){
    fond_err(OUT_OF_MEMORY);
    goto fond_compute_cleanup;
  }

  for(size_t i=0; i<size; ++i){
    stbtt_aligned_quad quad = {0};
    int index = fond_codepoint_index(font, text[i]);
    if(index < 0){
      fond_err(UNLOADED_GLYPH);
      goto fond_compute_cleanup;
    }
  
    stbtt_GetPackedQuad((stbtt_packedchar *)font->chardata, font->width, font->height, index, &x, &y, &quad, 1);
    int vi = 4*4*i;
    vert[vi++] = quad.x0; vert[vi++] = -quad.y1; vert[vi++] = quad.s0; vert[vi++] = quad.t1;
    vert[vi++] = quad.x0; vert[vi++] = -quad.y0; vert[vi++] = quad.s0; vert[vi++] = quad.t0;
    vert[vi++] = quad.x1; vert[vi++] = -quad.y0; vert[vi++] = quad.s1; vert[vi++] = quad.t0;
    vert[vi++] = quad.x1; vert[vi++] = -quad.y1; vert[vi++] = quad.s1; vert[vi++] = quad.t1;
  
    int ii = 2*3*i;
    ind[ii++] = i*4+0; ind[ii++] = i*4+1; ind[ii++] = i*4+3;
    ind[ii++] = i*4+1; ind[ii++] = i*4+2; ind[ii++] = i*4+3;
  }

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4*size, vert, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (GLvoid*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6*size, ind, GL_STATIC_DRAW);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if(glGetError() != GL_NO_ERROR){
    glDeleteVertexArrays(1, &vao);
    fond_err(OPENGL_ERROR);
    goto fond_compute_cleanup;
  }

  fond_err(NO_ERROR);
  *_n = 2*3*size;
  *_vao = vao;

 fond_compute_cleanup:
  if(vert)
    free(vert);

  if(ind)
    free(ind);

  if(vbo)
    glDeleteBuffers(2, &vbo);

  if(ebo)
    glDeleteBuffers(2, &ebo);

  return (errorcode == NO_ERROR);
}

int fond_compute_extent_u(struct fond_font *font, int32_t *text, size_t size, struct fond_extent *extent){
  int ascent, descent, linegap;
  stbtt_GetFontVMetrics(font->fontinfo, &ascent, &descent, &linegap);
  float scale = font->size / (ascent - descent);
  extent->t = ascent;
  extent->b = -descent;
  extent->gap = linegap;
  
  if(0 < size){
    int advance, bearing;
    stbtt_GetCodepointHMetrics(font->fontinfo, text[0], &advance, &bearing);
    extent->r = advance;
    extent->l = -bearing;
    for(size_t i=1; i<size; ++i){
      extent->r += stbtt_GetCodepointKernAdvance(font->fontinfo, text[i-1], text[i]);
      stbtt_GetCodepointHMetrics(font->fontinfo, text[i], &advance, &bearing);
      extent->r += advance;
    }
  }

  fond_err(NO_ERROR);
  extent->l *= scale;
  extent->r *= scale;
  extent->t *= scale;
  extent->b *= scale;
  extent->gap *= scale;
  return 1;
}
