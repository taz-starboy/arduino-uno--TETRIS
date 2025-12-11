#pragma once
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64

#define HORIZONTAL_MARGIN_PIXELS  2   // useful to determine start x game container
#define VERTICAL_MARGIN_PIXELS    4   // useful to determine start y game container
#define GAME_CONTAINER_WIDTH      62  // 60 + 1 pixel left + 1 pixel right
#define GAME_CONTAINER_HEIGHT     122 // 120 + 1 pixel top + 1 pixel bottom

#define OLED_RESET      -1
#define SCREEN_ADDRESS  0x3C

extern Adafruit_SSD1306 display;


#define GAME_OVER_TEXT_X  5
#define GAME_OVER_TEXT_Y  42
#define GAME_OVER_TEXT    "GAME OVER"
#define WINNING_TEXT_X    11
#define WINNING_TEXT_Y    42
#define WINNING_TEXT      "YOU WIN"


#define START_MAP_X 4
#define START_MAP_Y 4
extern uint8_t old_map_x; // START_MAP_X at game start
extern uint8_t old_map_y; // START_MAP_Y at game start

#define NEXT_TETROMINO_PREVIEW_X_COORDINATES 11
#define NEXT_TETROMINO_PREVIEW_Y_COORDINATES 1

#define POINTS_TEXT_X_COORDINATES 5
#define POINTS_TEXT_Y_COORDINATES 6
#define POINTS_X_COORDINATES      5
#define POINTS_Y_COORDINATES      17

#define BLOCK_SIZE                6
#define BLOCK_PREVIEW_SIZE        4
extern uint8_t block_size;          // BLOCK_SIZE
extern uint8_t block_preview_size;  // BLOCK_PREVIEW_SIZE