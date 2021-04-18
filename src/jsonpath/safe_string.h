#ifndef STRING_H
#define STRING_H 1

/**
 * Safely copy strings
 * Returns the total characters overflowed
 */
size_t jp_str_cpy(char* dst, size_t dst_size, char* src, size_t src_size);

#endif /* STRING_H */
