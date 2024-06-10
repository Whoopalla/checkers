#include "checkers.h"
#include <arpa/inet.h>
#include <byteprint.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define PRINT_BOARD(V)                                                         \
  do {                                                                         \
    uint32_t x = V;                                                            \
    print_board(#V, (unsigned char *)&x, sizeof(x));                           \
  } while (0)
int run(uint32_t *wp, uint32_t *bp, uint32_t *k);

void print_board_bits(char val) {
  printf("  ");
  for (int i = 7; i >= 0; i--) {
    if (i == 3)
      printf("\n");
    printf("%s   ", (val & (1 << i)) ? "1" : "0");
  }
}

void print_board(char *val, unsigned char *bytes, size_t num_bytes) {
  for (size_t i = 0; i < num_bytes; i++) {
    print_board_bits(bytes[i]);
    printf("\n");
  }
  printf("\n");
}

static uint32_t wp;
static uint32_t bp;
static uint32_t k;
static bool white_move = true;

static uint32_t mask_l3, mask_l5, mask_r3, mask_r5;
static uint32_t s[32];

/*
   28  29  30  31
 24  25  26  27
   20  21  22  23
 16  17  18  19
   12  13  14  15
 08  09  10  11
   04  05  06  07
 00  01  02  03
*/

static void brd_init() {
  wp = 0xFFF00000;
  bp = 0x00000FFF;
  k = 0;
}

static void masks_init() {
  s[0] = 1;
  for (int i = 1; i < 32; i++) {
    s[i] = s[i - 1] * 2;
  }
  mask_l3 = s[1] | s[2] | s[3] | s[9] | s[10] | s[11] | s[17] | s[18] | s[19] |
            s[25] | s[26] | s[27];
  mask_l5 = s[4] | s[5] | s[6] | s[12] | s[13] | s[14] | s[20] | s[21] | s[22];
  mask_r3 = s[28] | s[29] | s[30] | s[20] | s[21] | s[22] | s[12] | s[13] |
            s[14] | s[4] | s[5] | s[6];
  mask_r5 =
      s[25] | s[26] | s[27] | s[17] | s[18] | s[19] | s[9] | s[10] | s[11];
  mask_l3 = mask_l3;
  mask_l5 = mask_l5;
  mask_r3 = mask_r3;
  mask_r5 = mask_r5;
}

void checkers_init(void) {
  brd_init();
  masks_init();
}

uint32_t get_wp(void) { return wp; }
uint32_t get_bp(void) { return bp; }
uint32_t get_k(void) { return k; }

uint32_t get_white_movers() {
  const uint32_t nocc = ~(wp | bp);
  const uint32_t wk = wp & k;
  uint32_t movers = (nocc << 4) & wp;
  movers |= ((nocc & mask_l3) << 3) & wp;
  movers |= ((nocc & mask_l5) << 5) & wp;
  if (wk) {
    movers |= (nocc >> 4) & wk;
    movers |= ((nocc & mask_r3) >> 3) & wk;
    movers |= ((nocc & mask_r5) >> 5) & wk;
  }
  return movers;
}

uint32_t get_black_movers() {
  const uint32_t nooc = ~(wp | bp);
  const uint32_t bk = bp & k;
  uint32_t movers = (nooc >> 4) & bp;
  movers |= ((nooc & mask_l3) >> 3) & bp;
  movers |= ((nooc & mask_l5) >> 5) & bp;
  if (bp) {
    movers |= (nooc << 4) & bk;
    movers |= ((nooc & mask_r3) << 3) & bk;
    movers |= ((nooc & mask_l5) << 5) & bk;
  }
  return movers;
}

uint32_t get_white_jumpers() {
  const uint32_t nooc = ~(wp | bp);
  const uint32_t wk = wp & k;
  uint32_t movers = 0;
  uint32_t temp = (nooc << 4) & bp;
  if (temp) {
    movers |= (((temp & mask_l3 << 3) | ((temp & mask_l5) << 5))) & wp;
  }
  temp = (((nooc & mask_l3) << 3) | ((nooc & mask_l5) << 5)) & bp;
  movers |= (temp << 4) & wp;
  if (wk) {
    temp = (nooc >> 4) & bp;
    if (temp) {
      movers |= (((temp & mask_r3) >> 3) | ((temp & mask_r5) >> 5)) & wk;
    }
    temp = (((nooc & mask_r3) >> 3) | ((nooc & mask_r5) >> 5)) & bp;
    if (temp) {
      movers |= (temp >> 4) & wk;
    }
  }
  return movers;
}

void print_masks() {
  printf("Masks: ");
  PRINT_BITS(uint32_t, mask_l3);
  PRINT_BITS(uint32_t, mask_l5);
  PRINT_BITS(uint32_t, mask_r3);
  PRINT_BITS(uint32_t, mask_r5);
}

bool is_white_move(void) {
  return white_move;
}

// int main(void) {
//   uint32_t t = 1;
//
//   brd_init();
//   masks_init();
//
//   print_masks();
//
//   printf("wp: \n");
//   PRINT_BITS(uint32_t, wp);
//
//   printf("After htonl wp: \n");
//   wp = htonl(wp);
//   PRINT_BITS(uint32_t, wp);
//
//   printf("After second htonl wp: \n");
//   wp = htonl(wp);
//   PRINT_BITS(uint32_t, wp);
//
//   PRINT_BITS(uint32_t, wp);
//
//   printf("bp: \n");
//   PRINT_BITS(uint32_t, bp);
//
//   PRINT_BOARD(htonl(wp));
//   PRINT_BOARD(htonl(bp));
//
//   printf("Movers: \n");
//   PRINT_BITS(uint32_t, get_white_movers());
//   PRINT_BITS(uint32_t, get_black_movers());
// }
