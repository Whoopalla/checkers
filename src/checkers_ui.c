#include "checkers.h"
#include "utils/byteprint.h"
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800

#define CELL_SIZE 100

#define WHITE_CELL_COLOR GetColor(0xcad2c5ff)
#define BLACK_CELL_COLOR GetColor(0x2f3e46ff)
#define WHITE_P_COLOR (WHITE)
#define BLACK_P_COLOR (BLACK)

#define FLIP_CELL_COLOR(c)                                                     \
  (ColorIsEqual((c), WHITE_CELL_COLOR) ? (BLACK_CELL_COLOR)                    \
                                       : (WHITE_CELL_COLOR))

Game game;

uint32_t selected_piece = 0;
uint32_t piece_dest = 0;

void draw_board(void) {
  Color cell_color = BLACK_CELL_COLOR;
  uint32_t wp, bp, wm, bm, wj, bj, k, bitm;
  wp = game.wp;
  bp = game.bp;
  k = game.k;
  bitm = 1;

  if (game.white_move) {
    wm = get_white_movers(&game);
    bm = 0;
  } else {
    bm = get_black_movers(&game);
    wm = 0;
  }

  wm = get_white_movers(&game);
  bm = get_black_movers(&game);
  wj = get_white_jumpers(&game);
  //bj = get_black_jumpers(&game);

  for (int y = WIDTH - 100; y >= 0; y -= CELL_SIZE) {
    for (size_t x = 0; x < HEIGHT; x += CELL_SIZE) {
      DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cell_color);
      //char *pos = malloc(100);
      //sprintf(pos, "x: %d\n y: %d", x, y);
      //DrawText(pos, x, y, 20, RED);
      //free(pos);
      if (selected_piece != 0 && (selected_piece & bitm) &&
          ColorIsEqual(cell_color, BLACK_CELL_COLOR)) {
        DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, YELLOW);
      }
      if (piece_dest != 0 && (piece_dest & bitm) &&
          ColorIsEqual(cell_color, BLACK_CELL_COLOR)) {
        DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, RED);
      }

      if (ColorIsEqual(cell_color, BLACK_CELL_COLOR)) {
        if (wp & bitm) {
          DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 40, WHITE_P_COLOR);
          if (wm & bitm) {
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 20, GREEN);
          }
          if (wj & bitm) {
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 20, RED);
          }
        }
        if (bp & bitm) {
          DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 40, BLACK_P_COLOR);
          if (bm & bitm) {
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 20, GREEN);
          }
          //if (bj & bitm) {
          //  DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 20, RED);
          //}
        }
        bitm = bitm << 1;
      }
      cell_color = FLIP_CELL_COLOR(cell_color);
    }
    cell_color = FLIP_CELL_COLOR(cell_color);
  }
}

void get_input(void) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    Vector2 mouse = GetMousePosition();
    // printf("mouse x: %f y: %f\n", mouse.x, mouse.y);
    //  White cell click
    if (((int)mouse.y / CELL_SIZE % 2 != 0 &&
         (int)mouse.x / CELL_SIZE % 2 != 0) ||
        ((int)mouse.y / CELL_SIZE % 2 == 0 &&
         (int)mouse.x / CELL_SIZE % 2 == 0)) {
      selected_piece = 0;
      piece_dest = 0;
      return;
    }
    if (selected_piece == 0) {
      piece_dest = 0;
      uint32_t shift =
          ((int)mouse.x / CELL_SIZE) / 2 + 4 * (7 - (int)mouse.y / CELL_SIZE);
      selected_piece = 1 << shift;
    } else {
      uint32_t shift =
          ((int)mouse.x / CELL_SIZE) / 2 + 4 * (7 - (int)mouse.y / CELL_SIZE);
      piece_dest = 1 << shift;
      printf("Shift: %d\n", shift);
      move(&game, selected_piece, piece_dest);
      // PRINT_BITS(uint32_t, selected_piece);
      printf(" - selected\n");
      // PRINT_BITS(uint32_t, piece_dest);
      printf(" - dest\n");
      selected_piece = 0;
      piece_dest = 0;
      PRINT_BITS(uint32_t, game.wp | game.bp);
    }
  }
}

int main(void) {
  SetWindowState(FLAG_MSAA_4X_HINT);
  InitWindow(WIDTH, HEIGHT, "Checkers");
  checkers_init();
  new_game(&game);
  while (!WindowShouldClose()) {
    // Vector2 mouse = GetMousePosition();
    //  int shift = ((int)mouse.x/CELL_SIZE)/2 + 4*(7 - (int)mouse.y/CELL_SIZE);
    //  printf("mouse x: %f y: %f | shift: %d\n", mouse.x, mouse.y, shift);
    get_input();
    // printf("selected: ");
    // PRINT_BITS(uint32_t, selected_piece);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    draw_board();
    EndDrawing();
  }
  CloseWindow();
}
