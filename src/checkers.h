#include <stdbool.h>
#include <stdint.h>

struct game {
  uint32_t wp;
  uint32_t bp;
  uint32_t k;
  bool white_move;
  bool game_over;
};

typedef struct game Game;

void checkers_init(void);
void new_game(Game *g);
uint32_t get_white_movers(struct game *g);
uint32_t get_black_movers(struct game *g);
uint32_t get_white_jumpers(struct game *g);
uint32_t get_black_jumpers(struct game *g);
bool move(struct game *g, uint32_t s, uint32_t d);

void init(void);
