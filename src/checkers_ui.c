#include "checkers.h"
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800

#define CELL_SIZE 100

#define WHITE_CELL_COLOR GetColor(0xcad2c5ff)
#define BLACK_CELL_COLOR GetColor(0x2f3e46ff)
#define WHITE_P_COLOR WHITE
#define BLACK_P_COLOR BLACK

#define FLIP_CELL_COLOR(c)                                                     \
  (ColorIsEqual((c), WHITE_CELL_COLOR) ? (BLACK_CELL_COLOR)                    \
                                       : (WHITE_CELL_COLOR))

uint32_t selected_piece = 0;
uint32_t piece_dest = 0;

void draw_board(void) {
  Color cell_color = WHITE_CELL_COLOR;
  uint32_t wp, bp, wm, bm, k, bitm;
  wp = get_wp();
  bp = get_bp();
  k = get_k();
  bool white_move;
  bitm = 1;

  if (white_move) {
    wm = get_white_movers();
    bm = 0;
  } else {
    bp = get_black_movers();
    wm = 0;
  }

  for (size_t y = 0; y < WIDTH; y += CELL_SIZE) {
    for (size_t x = 0; x < HEIGHT; x += CELL_SIZE) {
      DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, cell_color);
      if (selected_piece & bitm) {
        DrawRectangle(x, y, CELL_SIZE-40, CELL_SIZE-40, GRAY);
      }

      if (ColorIsEqual(cell_color, BLACK_CELL_COLOR)) {
        if (wp & bitm) {
          DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 40, WHITE_P_COLOR);
          if (wm & bitm) {
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 20, GREEN);
          }
        }
        if (bp & bitm) {
          DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 40, BLACK_P_COLOR);
          if (bm & bitm) {
            printf("huh\n");
            DrawCircle(x + CELL_SIZE / 2, y + CELL_SIZE / 2, 20, GREEN);
          }
        }
        bitm = bitm << 1;
      }
      cell_color = FLIP_CELL_COLOR(cell_color);
    }
    cell_color = FLIP_CELL_COLOR(cell_color);
  }
}

void get_input(void) {
  if (IsKeyPressed(MOUSE_BUTTON_LEFT)) {
    if (selected_piece == 0) {
      Vector2 mouse = GetMousePosition();
      uint32_t shift =
          ((int)mouse.x / CELL_SIZE) / 2 + 4 * (7 - (int)mouse.y / CELL_SIZE);
      selected_piece = 1 << shift;
    } else {
      Vector2 mouse = GetMousePosition();
      uint32_t shift =
          ((int)mouse.x / CELL_SIZE) / 2 + 4 * (7 - (int)mouse.y / CELL_SIZE);
      piece_dest = 1 << shift;
    }
  }
}

int main(void) {
  SetWindowState(FLAG_MSAA_4X_HINT);
  InitWindow(WIDTH, HEIGHT, "Checkers");
  checkers_init();
  while (!WindowShouldClose()) {
    Vector2 mouse = GetMousePosition();
    // int shift = ((int)mouse.x/CELL_SIZE)/2 + 4*(7 - (int)mouse.y/CELL_SIZE);
    // printf("mouse x: %f y: %f | shift: %d\n", mouse.x, mouse.y, shift);
    BeginDrawing();
    ClearBackground(SKYBLUE);
    draw_board();
    EndDrawing();
  }
  CloseWindow();
}
