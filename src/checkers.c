#include "checkers.h"
#include <arpa/inet.h>
#include <assert.h>
#include <byteprint.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void print_masks();

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

void new_game(Game *game) {
  game->wp = (0x6FF00000);
  game->bp = (0x00000FFE);
  game->k = 0;
  game->white_move = true;
  game->game_over = false;
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
}

void checkers_init(void) { masks_init(); }

// TODO: Not showing white movers with epmty cell on the left
uint32_t get_white_movers(Game *g) {
  const uint32_t nocc = ~(g->wp | g->bp);
  const uint32_t wk = g->wp & g->k;
  uint32_t movers = (nocc << 4) & g->wp;
  movers |= ((nocc & mask_l3) << 3) & g->wp;
  movers |= ((nocc & mask_l5) << 5) & g->wp;
  if (wk) {
    movers |= (nocc >> 4) & wk;
    movers |= ((nocc & mask_r3) >> 3) & wk;
    movers |= ((nocc & mask_r5) >> 5) & wk;
  }
  return movers;
}

uint32_t get_black_movers(Game *g) {
  const uint32_t nooc = ~(g->wp | g->bp);
  const uint32_t bk = g->bp & g->k;
  uint32_t movers = (nooc >> 4) & g->bp;
  movers |= ((nooc & mask_r3) >> 3) & g->bp;
  movers |= ((nooc & mask_r5) >> 5) & g->bp;
  if (g->bp) {
    movers |= (nooc << 4) & bk;
    movers |= ((nooc & mask_l3) << 3) & bk;
    movers |= ((nooc & mask_l5) << 5) & bk;
  }
  return movers;
}

uint32_t get_white_jumpers(Game *g) {
  const uint32_t nooc = ~(g->wp | g->bp);
  const uint32_t wk = g->wp & g->k;
  uint32_t movers = 0;
  uint32_t temp = (nooc << 4) & g->bp;
  if (temp) {
    movers |= (((temp & mask_l3 << 3) | ((temp & mask_l5) << 5))) & g->wp;
  }
  temp = (((nooc & mask_l3) << 3) | ((nooc & mask_l5) << 5)) & g->bp;
  movers |= (temp << 4) & g->wp;
  if (wk) {
    temp = (nooc >> 4) & g->bp;
    if (temp) {
      movers |= (((temp & mask_r3) >> 3) | ((temp & mask_r5) >> 5)) & wk;
    }
    temp = (((nooc & mask_r3) >> 3) | ((nooc & mask_r5) >> 5)) & g->bp;
    if (temp) {
      movers |= (temp >> 4) & wk;
    }
  }
  return movers;
}

void move(Game *g, uint32_t s, uint32_t d) {
  assert(g->wp & s || g->bp & s && "Empty cell move");
  if (g->wp & s) {
    g->wp ^= s;
    g->wp |= d;
  } else if (g->bp & s) {
    g->bp ^= s;
    g->bp |= d;
  }
  PRINT_BOARD(htonl(g->bp) | htonl(g->wp));
}

void print_masks() {
  printf("Masks: ");
  PRINT_BITS(uint32_t, mask_l3);
  PRINT_BITS(uint32_t, mask_l5);
  PRINT_BITS(uint32_t, mask_r3);
  PRINT_BITS(uint32_t, mask_r5);
}
