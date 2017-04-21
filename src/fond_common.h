#ifndef __FOND_COMMON_H__
#define __FOND_COMMON_H__
#include <stdlib.h>
#include <stdint.h>

int errorcode;
void fond_err(int code);

unsigned char *fond_load_file(char *file);

#endif
