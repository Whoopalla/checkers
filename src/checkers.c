#include "checkers.h"
#include <arpa/inet.h>
#include <assert.h>
#include <byteprint.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define DEBUG_MODE 0
#define NUM_DIAG 13

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

static uint32_t diagonals[NUM_DIAG];

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
  game->wp = (0x60000000);
  game->bp = (0x00000000);
  game->k = 0x60000000;
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

  diagonals[0] = s[1] | s[4] | s[8];
  diagonals[1] = s[2] | s[5] | s[9] | s[12] | s[16];
  diagonals[2] = s[3] | s[6] | s[10] | s[13] | s[17] | s[20] | s[24];
  diagonals[3] = s[7] | s[11] | s[14] | s[18] | s[21] | s[25] | s[28];
  diagonals[4] = s[15] | s[19] | s[22] | s[26] | s[29];
  diagonals[5] = s[23] | s[27] | s[30];

  diagonals[6] = s[3] | s[7];
  diagonals[7] = s[2] | s[6] | s[11] | s[15];
  diagonals[8] = s[1] | s[5] | s[10] | s[14] | s[19] | s[23];
  diagonals[9] = s[0] | s[4] | s[9] | s[13] | s[18] | s[22] | s[27] | s[31];
  diagonals[10] = s[8] | s[12] | s[17] | s[21] | s[26] | s[30];
  diagonals[11] = s[16] | s[20] | s[25] | s[29];
  diagonals[12] = s[24] | s[28];

  printf("diagonals: \n");
  for (size_t i = 0; i < NUM_DIAG; i++) {
    PRINT_BITS(uint32_t, diagonals[i]);
  }
}

void checkers_init(void) { masks_init(); }

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
  uint32_t jumpers = 0;
  uint32_t temp = (nooc << 4) & g->bp;
  if (temp) {
    jumpers |= (((temp & mask_l3) << 3) | ((temp & mask_l5) << 5)) & g->wp;
  }
  temp = (((nooc & mask_l3) << 3) | ((nooc & mask_l5) << 5)) & g->bp;
  jumpers |= (temp << 4) & g->wp;
  if (wk) {
    temp = (nooc >> 4) & g->bp;
    if (temp) {
      jumpers |= (((temp & mask_r3) >> 3) | ((temp & mask_r5) >> 5)) & wk;
    }
    temp = (((nooc & mask_r3) >> 3) | ((nooc & mask_r5) >> 5)) & g->bp;
    if (temp) {
      jumpers |= (temp >> 4) & wk;
    }
  }
  return jumpers;
}

bool move(Game *g, uint32_t s, uint32_t d) {
  const uint32_t nooc = ~(g->wp | g->bp);
  if (!(nooc & d)) {
    printf("Incorrect destination seleceted\n");
    return false;
  }
  if ((g->white_move && (s & get_white_movers(g)) == 0) ||
      (!g->white_move && (s & get_black_movers(g)) == 0)) {
    printf("Incorrect piece seleceted\n");
    return false;
  }
  // King move
  if (s & g->k) {
    // King's pown move
    if (((d << 4) & s) || (((d & mask_l3) << 3) & s) ||
        (((d & mask_l5) << 5) & s || ((d >> 4) & s) ||
         (((d & mask_r3) >> 3) & s) || (((d & mask_r5) >> 5) & s))) {
      if (s & g->wp) {
        g->wp ^= s;
        g->wp |= d;
      } else {
        g->bp ^= s;
        g->bp |= d;
      }
      g->k ^= s;
      g->k |= d;
      // g->white_move = !g->white_move;
      return true;
    } else {
      // king move
      for (size_t i = 0; i < 13; i++) {
        if ((diagonals[i] & s) && (diagonals[i] & d)) {
          uint32_t m = s;
          if (s > d) {
            while ((m ^ d) != 0) {
              m = m >> 1;
              // Obsticle
              if ((g->wp | g->bp) & diagonals[i] & m) {
                return false;
              }
            }
          } else {
            while ((m ^ d) != 0) {
              m = m << 1;
              // Obsticle
              if ((g->wp | g->bp) & diagonals[i] & m) {
                return false;
              }
            }
          }
          if (s & g->wp) {
            g->wp ^= s;
            g->wp |= d;
          } else {
            g->bp ^= s;
            g->bp |= d;
          }
          g->k ^= s;
          g->k |= d;
          // g->white_move = !g->white_move;
          return true;
        }
      }
    }
  } else {
    if (g->white_move && (s & g->wp)) {
      if (((d << 4) & s) || (((d & mask_r3) << 3) & s) ||
          (((d & mask_r5) << 5) & s)) {
        g->bp ^= s;
        g->bp |= d;
        // g->white_move = !g->white_move;
        return true;
      }
    } else if (!g->white_move && (s & g->bp)) {
      if (((d >> 4) & s) || (((d & mask_r3) >> 3) & s) ||
          (((d & mask_r5) >> 5) & s)) {
        g->bp ^= s;
        g->bp |= d;
        // g->white_move = !g->white_move;
        return true;
      }
    }
  }
  return false;
}

void print_masks() {
  printf("Masks: ");
  PRINT_BITS(uint32_t, mask_l3);
  PRINT_BITS(uint32_t, mask_l5);
  PRINT_BITS(uint32_t, mask_r3);
  PRINT_BITS(uint32_t, mask_r5);
}
