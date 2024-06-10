#include <stdint.h>
#include <stdbool.h>
void checkers_init(void);
uint32_t get_wp(void);
uint32_t get_bp(void);
uint32_t get_k(void);
uint32_t get_white_movers(void);
uint32_t get_black_movers(void);
uint32_t get_white_jumpers(void);
uint32_t get_black_jumpers(void);
bool is_white_move(void);

void init(void);
