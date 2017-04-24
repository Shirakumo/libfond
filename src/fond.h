#ifndef __LIBFOND_H__
#define __LIBFOND_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#  define FOND_WIN
#  include <windows.h>
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

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

  enum fond_error{
    FOND_NO_ERROR,
    FOND_FILE_LOAD_FAILED,
    FOND_OUT_OF_MEMORY,
    FOND_FONT_PACK_FAILED,
    FOND_FONT_INIT_FAILED,
    FOND_OPENGL_ERROR,
    FOND_SIZE_EXCEEDED,
    FOND_NOT_LOADED,
    FOND_UTF8_CONVERSION_ERROR,
    FOND_UNLOADED_GLYPH,
    FOND_NO_CHARACTERS_OR_CODEPOINTS
  };
  
  struct fond_font{
    // Font info
    char *file;
    int index;
    float size;
    char *characters;
    int32_t *codepoints;
    // Buffer info
    unsigned int width;
    unsigned int height;
    unsigned int oversample;
    GLuint atlas;
    // Internal data
    void *fontdata;
    void *chardata;
    void *fontinfo;
    int converted_codepoints;
  };

  struct fond_buffer{
    struct fond_font *font;
    GLuint texture;
    unsigned int width;
    unsigned int height;
    // Internal data
    GLuint program;
    GLuint framebuffer;
  };

  struct fond_extent{
    float l;
    float r;
    float t;
    float b;
    float gap;
  };

  void fond_free(struct fond_font *font);
  int fond_load(struct fond_font *font);
  int fond_load_fit(struct fond_font *font, unsigned int max_size);
  int fond_compute(struct fond_font *font, char *text, size_t *n, GLuint *vao);
  int fond_compute_u(struct fond_font *font, int32_t *text, size_t size, size_t *n, GLuint *vao);
  int fond_compute_extent(struct fond_font *font, char *text, struct fond_extent *extent);
  int fond_compute_extent_u(struct fond_font *font, int32_t *text, size_t size, struct fond_extent *extent);

  void fond_free_buffer(struct fond_buffer *buffer);
  int fond_load_buffer(struct fond_buffer *buffer);
  int fond_render(struct fond_buffer *buffer, char *text, float x, float y, float *color);
  int fond_render_u(struct fond_buffer *buffer, int32_t *text, size_t size, float x, float y, float *color);

  int fond_decode_utf8(void *string, int32_t **decoded, size_t *size);
  enum fond_error fond_error();
  char *fond_error_string(enum fond_error error);
  
#ifdef __cplusplus
}
#endif
#endif
