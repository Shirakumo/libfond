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

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

  enum fond_error{
    NO_ERROR,
    FILE_LOAD_FAILED,
    OUT_OF_MEMORY,
    FONT_PACK_FAILED,
    FONT_INIT_FAILED,
    OPENGL_ERROR,
    SIZE_EXCEEDED,
    NOT_LOADED,
    UTF8_CONVERSION_ERROR,
    UNLOADED_GLYPH,
    NO_CHARACTERS_OR_CODEPOINTS
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
  int fond_render(struct fond_buffer *buffer, char *text, float *color);
  int fond_render_u(struct fond_buffer *buffer, int32_t *text, size_t size, float *color);

  int fond_decode_utf8(void *string, int32_t **decoded, size_t *size);
  int fond_error();
  char *fond_error_string(int error);
  
#ifdef __cplusplus
}
#endif
#endif
