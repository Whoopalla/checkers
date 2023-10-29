#include <stdio.h>
#include <stdint.h>

void print_byte_as_bits(char val) {
    for (int i = 7; 0 <= i; i--) {
        printf("%c", (val & (1 << i)) ? '1' : '0');
    }
}

void print_bits(char* ty, char* val, unsigned char* bytes, size_t num_bytes) {
    for (size_t i = 0; i < num_bytes; i++) {
        print_byte_as_bits(bytes[i]);
        printf(" ");
    }
    printf("\n");
}