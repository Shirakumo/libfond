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
  #include <stdint.h>
  #include <stdlib.h>

  // This enum contains all possible error codes
  // that libfond can produce.
  // 
  // See fond_error
  // See fond_error_string
  FOND_EXPORT enum fond_error{
    // Everything is OK.
    FOND_NO_ERROR,
    // The font file could not be read. Most likely
    // it doesn't exist or the permission is denied.
    FOND_FILE_LOAD_FAILED,
    // An allocation failed, probably due to OOM.
    FOND_OUT_OF_MEMORY,
    // Failed to pack the font into an atlas.
    // This most likely means that the atlas' size
    // is too small.
    FOND_FONT_PACK_FAILED,
    // Failed to parse the font file's information.
    // It's likely that the file you gave was not
    // a TTF.
    FOND_FONT_INIT_FAILED,
    // OpenGL signalled an error. Check stderr for
    // more information.
    FOND_OPENGL_ERROR,
    // The maximum size on fond_load_fit was reached.
    FOND_SIZE_EXCEEDED,
    // A render or compute function was called when
    // the font was not properly loaded yet.
    FOND_NOT_LOADED,
    // The UTF8 string could not be converted as it
    // is malformatted.
    FOND_UTF8_CONVERSION_ERROR,
    // An attempt was made to render/compute a glyph
    // that was not included in the list of glyphs
    // when the font was loaded.
    FOND_UNLOADED_GLYPH,
    // The fond_font struct did contain neither a
    // list of characters nor a list of codepoints.
    FOND_NO_CHARACTERS_OR_CODEPOINTS
  };

  // This is the primary struct that contains all
  // relevant information about a font. You must
  // allocate this struct yourself and make sure
  // that it is zeroed out before you do anything
  // with it.
  // Either stack allocate it with ={0}, or use
  // calloc. Not zeroing out will land you in a
  // world of pain.
  //
  // See fond_free
  // See fond_load
  // See fond_load_fit
  // See fond_compute
  // See fond_compute_u
  // See fond_compute_extent
  // See fond_compute_extent_u
  FOND_EXPORT struct fond_font{
    // Path to the TTF file.
    char *file;
    // The index of the font within the TTF file.
    // You probably don't need to set this.
    int index;
    // The vertical font size in pixels. If you
    // render it above this resolution, you'll
    // get a blurry mess.
    float size;
    // A UTF8 encoded string of characters that
    // this font instance will be able to render.
    // Must be null-terminated.
    char *characters;
    // An array of Unicode codepoints that this
    // font instance will be able to render. Must
    // be null-terminated. This is automatically
    // filled in for you, if it is NULL and the
    // characters field is provided instead.
    int32_t *codepoints;
    // The width of the glyph texture atlas.
    unsigned int width;
    // The height of the glyph texture atlas.
    unsigned int height;
    // How much oversampling should be done.
    // Higher oversampling might improve the
    // quality of the rendering, but will need
    // a much bigger atlas size:
    // width*oversampling,height*oversampling
    unsigned int oversample;
    // The OpenGL texture ID for the atlas.
    unsigned int atlas;
    // Internal data.
    void *fontdata;
    void *chardata;
    void *fontinfo;
    int converted_codepoints;
  };

  // This struct allows for convenience in
  // rendering, as it will render text for you
  // into a texture, which you can then render
  // like any other. Thus you won't need to
  // handle the actual rendering logic yourself.
  //
  // See fond_free_buffer
  // See fond_load_buffer
  // See fond_render
  // See fond_render_u
  FOND_EXPORT struct fond_buffer{
    // Pointer to the font that it renders.
    struct fond_font *font;
    // The OpenGL texture ID to which this
    // buffer renders to.
    unsigned int texture;
    // The width of the texture.
    unsigned int width;
    // The height of the texture.
    unsigned int height;
    // Internal data.
    unsigned int program;
    unsigned int framebuffer;
  };

  // This struct contains information about
  // the extents of a text.
  //
  // See fond_compute_extent
  // See fond_compute_extent_u
  FOND_EXPORT struct fond_extent{
    // How far to the left the text extends
    // from zero.
    float l;
    // How far to the right the text extends
    // from zero.
    float r;
    // How far up the text extends from its
    // baseline.
    float t;
    // How far down the text extends from
    // its baseline.
    float b;
    // The gap between lines of the text.
    float gap;
  };

  // Most functions in this API return an int,
  // which can be ither 1 or 0, with the former
  // representing success and the latter
  // failure. On failure, you should check
  // fond_error to see what went wrong.

  // Free all the data that was allocated
  // into the struct by fond_load*. This
  // will /not/ free the characters array,
  // the file array, or the codepoints array
  // if the codepoints array was not computed
  // by fond_load*.
  FOND_EXPORT void fond_free(struct fond_font *font);

  // Load the font struct and allocate the
  // necessary OpenGL data.
  // The following fields must be set in the
  // struct:
  //   file
  //   size
  //   width
  //   height
  //   characters or codepoints
  FOND_EXPORT int fond_load(struct fond_font *font);

  // Load the font struct, attempting to fit
  // an atlas automatically. This may not
  // result in the most compact atlas possible.
  // max_size is the maximum size of the width
  // or height that can be reached before it
  // gives up. The following fields must be set
  // in the struct:
  //   file
  //   size
  //   characters or codepoints
  FOND_EXPORT int fond_load_fit(struct fond_font *font, unsigned int max_size);

  // Compute the Vertex Array Object to render
  // the given text. Here, n and vao are output
  // arguments, containing the number of elements
  // and the OpenGL VAO ID respectively.
  // The text must be UTF8 encoded and null-
  // terminated.
  //
  // The VAO and necessary VBOs are allocated
  // automatically for you and then filled in as
  // per fond_update.
  FOND_EXPORT int fond_compute(struct fond_font *font, char *text, size_t *n, unsigned int *vao);

  // Same as fond_compute, but taking an UTF32
  // encoded string of codepoints and its size.
  FOND_EXPORT int fond_compute_u(struct fond_font *font, int32_t *text, size_t size, size_t *n, unsigned int *vao);

  // Update the given vertex buffer and element
  // buffers to contain the necessary data to draw
  // the given text. The VBO packs two arrays, one
  // at location 0 and one at 1, with both being
  // vec2s. The first being the vertex coordinates
  // and the second being the texture coordinates.
  // The vertex coordinates start at 0 and increase
  // in x and y as per the font's size. You are
  // responsible for scaling it as appropriate
  // for your display.
  // The texture coordinates are for the font's
  // atlas texture.
  // If the text contains a Linefeed character
  // (U+000A) a new line is started automatically
  // by resetting X to 0 and decreasing Y by the
  // necessary height for a new line.
  FOND_EXPORT int fond_update(struct fond_font *font, char *text, size_t *n, unsigned int vbo, unsigned int ebo);

  // Same as fond_update, but taking an UTF32
  // encoded string of codepoints and its size.
  FOND_EXPORT int fond_update_u(struct fond_font *font, int32_t *text, size_t size, size_t *n, unsigned int vbo, unsigned int ebo);

  // Compute the extent of the given text.
  // You must allocate the extent struct yourself
  // and make sure it is zeroed out.
  FOND_EXPORT int fond_compute_extent(struct fond_font *font, char *text, struct fond_extent *extent);

  // Same as fond_compute_extent, but taking an
  // UTF32 encoded string of codepoints and its
  // size.
  FOND_EXPORT int fond_compute_extent_u(struct fond_font *font, int32_t *text, size_t size, struct fond_extent *extent);

  // Free all the data that was allocated
  // into the struct by fond_load_buffer. This
  // will /not/ free the font struct.
  FOND_EXPORT void fond_free_buffer(struct fond_buffer *buffer);

  // Load the buffer struct and allocate the
  // necessary OpenGL data.
  // The following fields must be set in the
  // struct:
  //   font
  //   width
  //   height
  FOND_EXPORT int fond_load_buffer(struct fond_buffer *buffer);

  // Render the given text to the buffer's
  // texture. The text will be rendered at the
  // given offset, with x and y being in pixels.
  // color can be either 0 for white text, or
  // an array of four floats, representing RGBA
  // of the text's colour in that order.
  FOND_EXPORT int fond_render(struct fond_buffer *buffer, char *text, float x, float y, float *color);

  // Same as fond_render, but taking an UTF32
  // encoded string of codepoints and its size.
  FOND_EXPORT int fond_render_u(struct fond_buffer *buffer, int32_t *text, size_t size, float x, float y, float *color);

  // Decode the given UTF8 string into an UTF32
  // string. The resulting string is put into
  // decoded, and its size is put into size.
  // This is used by the non _u functions to
  // decode the string. You may want to use this
  // internally, if you need to re-use the same
  // string often and don't wnat to pay the
  // conversion cost.
  FOND_EXPORT int fond_decode_utf8(void *string, int32_t **decoded, size_t *size);

  // Return the current error code.
  FOND_EXPORT enum fond_error fond_error();

  // Return a string for a human-readable error
  // message of the given error code.
  FOND_EXPORT char *fond_error_string(enum fond_error error);
  
#ifdef __cplusplus
}
#endif
#endif
