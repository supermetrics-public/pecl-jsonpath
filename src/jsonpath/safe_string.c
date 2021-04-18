#include "string.h"

size_t jp_str_cpy(char* dst, size_t dst_size, char* src, size_t src_size) {
  size_t len, overflow = 0;

  if (src_size > dst_size) {
    len = dst_size;
    overflow = src_size - dst_size;
  } else {
    len = src_size;
  }

  memcpy(dst, src, len);

  dst[len] = '\0';

  return overflow;
}
