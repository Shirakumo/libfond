#include "fond.h"
#include "fond_common.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int errorcode = 0;

unsigned char *fond_load_file(char *file){
  unsigned int size = 512;

  FILE *fd = fopen(file, "r");
  if(!fd) return 0;

  struct stat finfo;
  if(fstat(fileno(fd), &finfo)) return 0;
  
  unsigned char *data = calloc(finfo.st_size, sizeof(char));
  if(!data) return 0;

  if(fread(data, sizeof(char), finfo.st_size, fd) < finfo.st_size){
    free(data);
    return 0;
  }
  
  return data;
}

int fond_error(){
  return errorcode;
}

char *fond_error_string(int error){
  switch(error){
  case NO_ERROR:
    return "No error has occurred yet.";
  case FILE_LOAD_FAILED:
    return "Failed to load the font file. The file does not exist or is not accessible.";
  case OUT_OF_MEMORY:
    return "Allocation failure due to heap exhaustion.";
  case FONT_PACK_FAILED:
    return "Failed to pack the font. The atlas size was too small or the font file was invalid.";
  case OPENGL_ERROR:
    return "An OpenGL error has been encountered.";
  case SIZE_EXCEEDED:
    return "Maximum size for font loading has been exceeded.";
  case NOT_LOADED:
    return "Cannot render as the font has not been loaded properly yet.";
  case UTF8_CONVERSION_ERROR:
    return "Failed to convert UTF8 string to UTF32. It is probably malformatted.";
  case UNLOADED_GLYPH:
    return "A glyph that was not loaded into the font was attempted to be drawn.";
  default:
    return "Unknown error code.";
  }
}
