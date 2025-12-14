#pragma once
#include <stdio.h>

#define PAUSE 1000          // change this to accelerate / decelerate automatic movement
#define POINTS_TO_WIN 200

extern uint8_t points;

struct Tetromino {
  uint8_t current[8];
  uint8_t max_rotations;
};
struct ButtonState {
  bool held : 1;
  uint32_t pressTime = 0;
  uint32_t lastRepeat = 0;
};

bool buttonAction(uint8_t pin, ButtonState *button);
Tetromino getTetrominoCoordinates(uint8_t tetromino_number, uint8_t tetromino_rotation);
Tetromino rotateTetromino(uint8_t *map_x, uint8_t map_y, Tetromino tetromino, uint8_t tetromino_number, uint8_t *tetromino_rotation, uint8_t game_map[20][10]);
bool validateRotation(uint8_t *map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]);
bool canGoFurtherDown(uint8_t x, uint8_t y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]);
bool canGoLeft(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]);
bool canGoRight(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]);
void printOnMap(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]);
void drawTetromino(uint8_t map_x, uint8_t map_y, uint8_t block_size, const uint8_t tetromino_coordinates[8]);
void cancelTetromino(uint8_t map_x, uint8_t map_y, uint8_t block_size, const uint8_t tetromino_coordinates[8]);
void checkCompleteRow(uint8_t game_map[20][10]);
void cancelRow(uint8_t row_complete, uint8_t game_map[20][10]);
void printBlocks(uint8_t row_number, uint8_t row[10]);
void updatePoints(uint8_t x_points, uint8_t y_points, uint8_t *points);
bool checkGameOver(uint8_t game_map[20][10], uint8_t next_tetromino_number);
uint8_t generateRandomNumber();