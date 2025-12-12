#include <arduino.h>
#include "game_logic.h"
#include "display.h"
#include "tetrominoes_coordinates.h"


Tetromino getTetrominoCoordinates(uint8_t tetromino_number, uint8_t tetromino_rotation) {
  Tetromino tetromino;

  switch (tetromino_number) {
    case 0: // square      
      memcpy_P(tetromino.current, SQUARE_COORDINATES, 8);
      tetromino.max_rotations = 0;
      break;
    case 1: // s
      memcpy_P(tetromino.current, S_COORDINATES[tetromino_rotation], 8);
      tetromino.max_rotations = 2;
      break;
    case 2:
      memcpy_P(tetromino.current, Z_COORDINATES[tetromino_rotation], 8);
      tetromino.max_rotations = 2;
      break;
    case 3:
      memcpy_P(tetromino.current, L_COORDINATES[tetromino_rotation], 8);
      tetromino.max_rotations = 4;
      break;
    case 4:
      memcpy_P(tetromino.current, J_COORDINATES[tetromino_rotation], 8);
      tetromino.max_rotations = 4;
      break;
    case 5:
      memcpy_P(tetromino.current, T_COORDINATES[tetromino_rotation], 8);
      tetromino.max_rotations = 4;
      break;
    case 6:
      memcpy_P(tetromino.current, I_COORDINATES[tetromino_rotation], 8);
      tetromino.max_rotations = 2;
      break;
    default:
      Serial.println(F("No match for a tetromino."));
  }
  return tetromino;
}
// rotation occurs on same y position 
Tetromino rotateTetromino(uint8_t *map_x, uint8_t map_y, Tetromino tetromino, uint8_t tetromino_number, uint8_t *tetromino_rotation, uint8_t game_map[20][10]) { 
  uint8_t new_rotation = *tetromino_rotation;
  new_rotation++;

  if (new_rotation >= tetromino.max_rotations) new_rotation = 0;

  Tetromino turned_tetromino = getTetrominoCoordinates(tetromino_number, new_rotation);
  bool okToRotate = validateRotation(map_x, map_y, turned_tetromino.current, game_map);
  if (okToRotate) {
    cancelTetromino(old_map_x, old_map_y, block_size, tetromino.current);
    drawTetromino(*map_x, old_map_y, block_size, turned_tetromino.current);
    *tetromino_rotation = new_rotation;    
    return turned_tetromino;
  }
  return tetromino; 
}
bool validateRotation(uint8_t *map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    int8_t x_value = (*map_x) + tetromino_coordinates[i];
    uint8_t y_value = map_y + tetromino_coordinates[i + 1];
    if (x_value < 0) return false;
    if (x_value > 9) {
      return validateRotation(&(--(*map_x)), map_y, tetromino_coordinates, game_map);
    }
    if (game_map[y_value][x_value] == 1) return false;
  }
  return true;
}
bool canGoFurtherDown(uint8_t x, uint8_t y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t x_value = x + tetromino_coordinates[i];
    uint8_t y_value = y + tetromino_coordinates[i + 1];
    if (y_value == 19) return false;
    if (game_map[y_value + 1][x_value] == 1) return false;
  }
  return true;
}
bool canGoLeft(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]) {
  if (map_x < 1) return false;
  for (uint8_t i = 0; i < 8; i += 2) {
    int8_t x_value = map_x + tetromino_coordinates[i];
    if (x_value < 1) return false;
    uint8_t y_value = map_y + tetromino_coordinates[i + 1];    
    if (game_map[y_value][x_value - 1] == 1) return false;
  }
  return true;
}
bool canGoRight(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]) {
  if (map_x > 8) return false;
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t x_value = map_x + tetromino_coordinates[i];
    if (x_value > 8) return false;
    uint8_t y_value = map_y + tetromino_coordinates[i + 1];    
    if (game_map[y_value][x_value + 1] == 1) return false;
  }
  return true;
}
void printOnMap(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8], uint8_t game_map[20][10]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t var_x = map_x + tetromino_coordinates[i];
    uint8_t var_y = map_y + tetromino_coordinates[i + 1]; 
    game_map[var_y][var_x] = 1;
  }
}
void drawTetromino(uint8_t map_x, uint8_t map_y, uint8_t block_size, const uint8_t tetromino_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * block_size;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * block_size;
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + tetromino_coordinates[i] * block_size;
    uint8_t start_y = y + tetromino_coordinates[i + 1] * block_size;
    display.drawRect(start_x, start_y, block_size, block_size, WHITE);
  }
  display.display();
}
void cancelTetromino(uint8_t map_x, uint8_t map_y, uint8_t block_size, const uint8_t tetromino_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * block_size;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * block_size;  
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + tetromino_coordinates[i] * block_size;
    uint8_t start_y = y + tetromino_coordinates[i + 1] * block_size;
    display.drawRect(start_x, start_y, block_size, block_size, BLACK);
  }
}
void checkCompleteRow(uint8_t game_map[20][10]) {
  for (int8_t i = 19; i >= 0; i--) {
    for (uint8_t j = 0; j < 10; j++) {
      if (game_map[i][j] == 0) break;
      if (j == 9) cancelRow(i, game_map);
    }    
  }
}
void cancelRow(uint8_t row_complete, uint8_t game_map[20][10]) {
  for (uint8_t i = row_complete; i > START_MAP_Y; i--) {
    memcpy(game_map[i], game_map[i - 1], 10 * sizeof(uint8_t));
    printBlocks(i, game_map[i]);
  }  
  memset(game_map[START_MAP_Y], 0, 10 * sizeof(uint8_t));

  updatePoints(POINTS_X_COORDINATES, POINTS_Y_COORDINATES, &points);
}
void printBlocks(uint8_t row_number, uint8_t row[10]) {
  uint8_t y = VERTICAL_MARGIN_PIXELS + row_number * block_size;
  for (uint8_t i = 0; i < 10; i++) {
    uint8_t x = HORIZONTAL_MARGIN_PIXELS + i * block_size;
    if (row[i] == 1) {      
      display.drawRect(x, y, block_size, block_size, WHITE);
      continue;
    }
    display.drawRect(x, y, block_size, block_size, BLACK);
  }
}
void updatePoints(uint8_t x_points, uint8_t y_points, uint8_t *points) {
  (*points) += 10; 
  display.fillRect(x_points, y_points, 35, 7, BLACK);
  display.setCursor(x_points, y_points);
  display.print(*points);  
}
bool checkGameOver(uint8_t game_map[20][10], uint8_t next_tetromino_number) {
  if ((game_map[4][4] || game_map[4][5] || game_map[5][4] || game_map[5][5]) && next_tetromino_number != 0) return true;
  return false;
}
uint8_t generateRandomNumber() {
  return random(0, 7);
}