#include <stdint.h>
#include <stddef.h>

int is_valid_char(uint32_t ch){
  return ch < 0xd800 || ch > 0xdfff;
}

static int getch(uint8_t buf[], unsigned long *idx, size_t strlen, uint32_t *cp){
  int remunits;
  uint8_t nxt, msk;
  if (*idx >= strlen)
    return 0;
  nxt = buf[(*idx)++];
  if (nxt & 0x80) {
    msk = 0xe0;
    for (remunits = 1; (nxt & msk) != (msk << 1); ++remunits)
      msk = (msk >> 1) | 0x80;
  } else {
    remunits = 0;
    msk = 0;
  }
  *cp = nxt ^ msk;
  while (remunits-- > 0) {
    *cp <<= 6;
    if (*idx >= strlen)
      return 0;
    *cp |= buf[(*idx)++] & 0x3f;
  }
  return 1;
}

int utf8_to_utf32(uint8_t input[], uint32_t output[], size_t count, size_t *out_size){
  unsigned long idx = 0;
  for (*out_size = 0; *out_size < count; ++*out_size) {
    if (!getch(input, &idx, count, &output[idx]))
      return 0;
    if (!is_valid_char(output[idx]))
      return 0;
  }
  return 1;
}
