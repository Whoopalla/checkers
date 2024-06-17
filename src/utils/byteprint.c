#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>

void print_byte_as_bits(char val) {
  for (int i = 7; 0 <= i; i--) {
    printf("%c", (val & (1 << i)) ? '1' : '0');
  }
}

// TODO: works only on little endian
void print_bits(char *ty, char *val, unsigned char *bytes, size_t num_bytes) {
    for (int i = num_bytes-1; i >= 0; i--) {
    print_byte_as_bits(bytes[i]);
    printf(" ");
    }
    printf("\n");
}
