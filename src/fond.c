#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STBTT_STATIC
#define STBRP_STATIC
#define LINEFEED 0x000A

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "fond_internal.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"

FOND_EXPORT void fond_free(struct fond_font *font){
  if(font->allocated_atlas && font->atlas)
    glDeleteTextures(1, &font->atlas);
  font->atlas = 0;
  font->allocated_atlas = 0;
  
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
    fond_err(FOND_NO_CHARACTERS_OR_CODEPOINTS);
    return 0;
  }
  
  font->chardata = calloc(size, sizeof(stbtt_packedchar));
  if(!font->chardata){
    if(font->chardata)
      free(font->chardata);
    font->chardata = 0;
    fond_err(FOND_OUT_OF_MEMORY);
    return 0;
  }
  
  fond_err(FOND_NO_ERROR);
  range->font_size = font->size;
  range->array_of_unicode_codepoints = (int *)font->codepoints;
  range->num_chars = size;
  range->chardata_for_range = font->chardata;
  return 1;
}

int fond_load_internal(struct fond_font *font, stbtt_pack_range *range){
  stbtt_pack_context context = {0};
  unsigned char *atlasdata = calloc(font->width*font->height, sizeof(char));

#ifdef FOND_WIN
  fond_load_glext();
#endif

  if(!atlasdata){
    fond_err(FOND_OUT_OF_MEMORY);
    goto fond_load_internal_cleanup;
  }

  font->fontinfo = calloc(1, sizeof(stbtt_fontinfo));

  if(!stbtt_InitFont(font->fontinfo, font->fontdata, stbtt_GetFontOffsetForIndex(font->fontdata, font->index))){
    fond_err(FOND_FONT_INIT_FAILED);
    goto fond_load_internal_cleanup;
  }
  
  if(!stbtt_PackBegin(&context, atlasdata, font->width, font->height, 0, 1, 0)){
    fond_err(FOND_FONT_PACK_FAILED);
    goto fond_load_internal_cleanup;
  }

  if(0 < font->oversample)
    stbtt_PackSetOversampling(&context, font->oversample, font->oversample);

  if(!stbtt_PackFontRanges(&context, font->fontdata, font->index, range, 1)){
    fond_err(FOND_FONT_PACK_FAILED);
    goto fond_load_internal_cleanup;
  }

  stbtt_PackEnd(&context);

  if(font->atlas == 0){
    font->allocated_atlas = 1;
    glGenTextures(1, &font->atlas);
    glBindTexture(GL_TEXTURE_2D, font->atlas);
    glTexStorage2D(GL_TEXTURE_2D, 8, GL_R8, font->width, font->height);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.65);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  }else{
    glBindTexture(GL_TEXTURE_2D, font->atlas);
  }

  GLint unpack;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, font->width, font->height, GL_RED, GL_UNSIGNED_BYTE, atlasdata);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

  if(!fond_check_glerror()){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_load_internal_cleanup;
  }
  
  fond_err(FOND_NO_ERROR);
  return 1;

 fond_load_internal_cleanup:
  if(atlasdata)
    free(atlasdata);
  
  if(font->fontinfo)
    free(font->fontinfo);
  font->fontinfo = 0;
  
  if(font->allocated_atlas && font->atlas){
    glDeleteTextures(1, &font->atlas);
    font->atlas = 0;
  }
  font->allocated_atlas = 0;
  return 0;
}

FOND_EXPORT int fond_load(struct fond_font *font){
  stbtt_pack_range range = {0};
  
  if(!fond_load_file(font->file, &font->fontdata)){
    fond_err(FOND_FILE_LOAD_FAILED);
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

FOND_EXPORT int fond_load_fit(struct fond_font *font, unsigned int max_size){
  stbtt_pack_range range = {0};

  if(!fond_load_file(font->file, &font->fontdata)){
    fond_err(FOND_FILE_LOAD_FAILED);
    goto fond_load_fit_cleanup;
  }

  if(!fond_pack_range(font, &range)){
    goto fond_load_fit_cleanup;
  }
  
  if(font->width == 0) font->width = 64;
  if(font->height == 0) font->height = 64;
  
  while(font->width <= max_size
        && font->height <= max_size){
    if(fond_load_internal(font, &range)){
      return 1;
    }

    switch(errorcode){
    case FOND_FONT_INIT_FAILED:
    case FOND_OPENGL_ERROR:
    case FOND_OUT_OF_MEMORY:
      goto fond_load_fit_cleanup;
    case FOND_FONT_PACK_FAILED:
      font->width *= 2;
      font->height *= 2;
    }
  }
  
  fond_err(FOND_SIZE_EXCEEDED);

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

FOND_EXPORT int fond_compute_u(struct fond_font *font, int32_t *text, size_t size, size_t *_n, GLuint *_vao){
  GLuint vao = 0, vbo = 0, ebo = 0;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (GLvoid*)(2*sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if(!fond_check_glerror()){
    glDeleteVertexArrays(1, &vao);
    fond_err(FOND_OPENGL_ERROR);
    goto fond_compute_cleanup;
  }
  
  if(!fond_update_u(font, text, size, _n, vbo, ebo)){
    goto fond_compute_cleanup;
  }

  *_vao = vao;

 fond_compute_cleanup:
  if(vbo)
    glDeleteBuffers(1, &vbo);

  if(ebo)
    glDeleteBuffers(1, &ebo);

  return (errorcode == FOND_NO_ERROR);
}

FOND_EXPORT int fond_update_u(struct fond_font *font, int32_t *text, size_t size, size_t *_n, GLuint vbo, GLuint ebo){
  GLfloat *vert = 0;
  GLuint *ind = 0;
  float x = 0, y = 0;
  
  if(!font->chardata){
    fond_err(FOND_NOT_LOADED);
    goto fond_update_cleanup;
  }

  vert = calloc(4*4*size, sizeof(GLfloat));
  ind = calloc(2*3*size, sizeof(GLuint));
  if(!vert || !ind){
    fond_err(FOND_OUT_OF_MEMORY);
    goto fond_update_cleanup;
  }

  int ascent, descent, linegap;
  stbtt_GetFontVMetrics(font->fontinfo, &ascent, &descent, &linegap);
  float scale = font->size / (ascent - descent);
  float vskip = scale*(linegap+ascent-descent);

  for(size_t i=0; i<size; ++i){
    stbtt_aligned_quad quad = {0};
    int index = fond_codepoint_index(font, text[i]);
    if(index < 0){
      printf("FOND: Unloaded glyph %c (U+%x) at %i\n", text[i], text[i], text[i]);
      fond_err(FOND_UNLOADED_GLYPH);
      goto fond_update_cleanup;
    }
  
    stbtt_GetPackedQuad((stbtt_packedchar *)font->chardata, font->width, font->height, index, &x, &y, &quad, 1);
    int vi = 4*4*i;
    if(text[i] == LINEFEED){
      vert[vi++] = quad.x0; vert[vi++] = -quad.y1; vert[vi++] = 0; vert[vi++] = 0;
      vert[vi++] = quad.x0; vert[vi++] = -quad.y0; vert[vi++] = 0; vert[vi++] = 0;
      vert[vi++] = quad.x1; vert[vi++] = -quad.y0; vert[vi++] = 0; vert[vi++] = 0;
      vert[vi++] = quad.x1; vert[vi++] = -quad.y1; vert[vi++] = 0; vert[vi++] = 0;
      y += vskip;
      x = 0;
    }else{
      vert[vi++] = quad.x0; vert[vi++] = -quad.y1; vert[vi++] = quad.s0; vert[vi++] = quad.t1;
      vert[vi++] = quad.x0; vert[vi++] = -quad.y0; vert[vi++] = quad.s0; vert[vi++] = quad.t0;
      vert[vi++] = quad.x1; vert[vi++] = -quad.y0; vert[vi++] = quad.s1; vert[vi++] = quad.t0;
      vert[vi++] = quad.x1; vert[vi++] = -quad.y1; vert[vi++] = quad.s1; vert[vi++] = quad.t1;
    }
  
    int ii = 2*3*i;
    ind[ii++] = i*4+0; ind[ii++] = i*4+3; ind[ii++] = i*4+1;
    ind[ii++] = i*4+1; ind[ii++] = i*4+3; ind[ii++] = i*4+2;
  }

  // This is done as a sanity option, as otherwise we might rebind the
  // buffers of a bound VAO in the outermost scope. Properly doing this
  // would require getting the currently bound values and restoring and
  // all that, but I think it's fine to not do that here.
  glBindVertexArray(0);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*4*size, vert, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6*size, ind, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  if(!fond_check_glerror()){
    fond_err(FOND_OPENGL_ERROR);
    goto fond_update_cleanup;
  }

  fond_err(FOND_NO_ERROR);
  *_n = 2*3*size;

 fond_update_cleanup:
  if(vert)
    free(vert);

  if(ind)
    free(ind);

  return (errorcode == FOND_NO_ERROR);
}

FOND_EXPORT int fond_compute_extent_u(struct fond_font *font, int32_t *text, size_t size, struct fond_extent *extent){
  int ascent, descent, linegap, advance, bearing;
  stbtt_GetFontVMetrics(font->fontinfo, &ascent, &descent, &linegap);
  float scale = font->size / (ascent - descent);
  float vskip = scale*(linegap+ascent-descent);
  extent->l = 0;
  extent->r = 0;
  extent->t = ascent;
  extent->b = -descent;
  extent->gap = linegap;
  
  if(0 < size){
    int right = 0;
    size_t i=0;
    // Get left extent by finding first real character.
    for(; i<size; ++i){
      if(text[i] == LINEFEED){
        extent->b += vskip;
      }else{
        stbtt_GetCodepointHMetrics(font->fontinfo, text[i], &advance, &bearing);
        right += advance;
        extent->l = -bearing;
        break;
      }
    }
    // Compute the maximal right and bottom extent.
    for(; i<size; ++i){
      if(text[i] == LINEFEED){
        extent->b += vskip;
        extent->r = (extent->r > right)? extent->r : right;
        right = 0;
      }else{
        right += stbtt_GetCodepointKernAdvance(font->fontinfo, text[i-1], text[i]);
        stbtt_GetCodepointHMetrics(font->fontinfo, text[i], &advance, &bearing);
        right += advance;
      }
    }
    extent->r = (extent->r > right)? extent->r : right;
  }

  fond_err(FOND_NO_ERROR);
  extent->l *= scale;
  extent->r *= scale;
  extent->t *= scale;
  extent->b *= scale;
  extent->gap *= scale;
  return 1;
}
