#ifndef __LIBFOND_H__
#define __LIBFOND_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#  ifdef FOND_STATIC_DEFINE
#    define FOND_EXPORT
#  else
#    ifndef FOND_EXPORT
#      define FOND_EXPORT __declspec(dllexport)
#    endif
#  endif
#else
#  define FOND_EXPORT
#endif

#include <GL/gl.h>
#include <GL/glext.h>
  
  struct fond_font{
    // Font info
    char *file;
    int index;
    float size;
    char *characters;
    uint32_t *codepoints;
    // Buffer info
    unsigned int width, height;
    unsigned int oversample_h, oversample_v;
    GLuint texture;
    // Internal data
    void *data;
    int converted_codepoints;
  };

  struct fond_buffer{
    struct fond_font *font;
    GLuint texture;
    unsigned int width, height;
    // Internal data
    GLuint program, framebuffer;
  };

  enum fond_error{
    NO_ERROR,
    FILE_LOAD_FAILED,
    OUT_OF_MEMORY,
    FONT_PACK_FAILED,
    OPENGL_ERROR,
    SIZE_EXCEEDED,
    NOT_LOADED,
    UTF8_CONVERSION_ERROR,
    UNLOADED_GLYPH
  };

  void fond_free(struct fond_font *font);
  int fond_load(struct fond_font *font);
  int fond_load_fit(struct fond_font *font, unsigned int max_size);
  int fond_compute(struct fond_font *font, char *text, unsigned int *n, float *x, float *y, GLuint *vao);

  void fond_free_buffer(struct fond_buffer *buffer);
  int fond_load_buffer(struct fond_buffer *buffer);
  int fond_render(struct fond_buffer *buffer, char *text);
  
  int fond_error();
  char *fond_error_string(int error);
  
#ifdef __cplusplus
}
#endif
#endif
