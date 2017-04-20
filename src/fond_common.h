#ifndef __FOND_COMMON_H__
#define __FOND_COMMON_H__
#include <stdlib.h>
#include <stdint.h>

int errorcode;

int fond_decode_utf8(void *string, int32_t **decoded, size_t *size);
unsigned char *fond_load_file(char *file);

#endif
