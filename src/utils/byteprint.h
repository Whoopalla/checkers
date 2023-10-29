#include <stdio.h>

#define PRINT_BITS(T, V)                                   \
    do {                                                   \
        T x = V;                                           \
        print_bits(#T, #V, (unsigned char*)&x, sizeof(x)); \
    } while (0)

void print_byte_as_bits(char val);

void print_bits(char* ty, char* val, unsigned char* bytes, size_t num_bytes);
