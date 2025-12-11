#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "tetrominoes_coordinates.h"
#include "game_logic.h"
#include "display.h"


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_DOWN     2
#define BUTTON_LEFT     3
#define BUTTON_RIGHT    4
#define BUTTON_ROTATION 5
#define BUZZER          13

uint8_t block_size = BLOCK_SIZE;
uint8_t block_preview_size = BLOCK_PREVIEW_SIZE;

uint8_t map_x = START_MAP_X;
uint8_t map_y = START_MAP_Y;
uint8_t old_map_x = map_x;
uint8_t old_map_y = map_y;

Tetromino tetromino;
uint8_t tetromino_number;          
Tetromino next_tetromino;
uint8_t next_tetromino_number;
uint8_t tetromino_rotation = 0;

uint8_t game_map[20][10] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

uint8_t points = 0;
bool is_game_over = false;

uint32_t last_time = millis();

// **** START ****
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // setting pins
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_ROTATION, INPUT);
  pinMode(BUZZER, OUTPUT);  
  
  // setting and update display
  display.clearDisplay();
  display.setRotation(1); // set display to vertical orientation  

  // draw game table
  display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, GAME_CONTAINER_WIDTH, GAME_CONTAINER_HEIGHT, WHITE); 
  display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, GAME_CONTAINER_WIDTH, START_MAP_Y * block_size + 1, WHITE);

  randomSeed(analogRead(A0)); // initialize random seed
  tetromino_number = generateRandomNumber(); // move to loop to avoid generating 0 as first
  tetromino = getTetrominoCoordinates(tetromino_number, tetromino_rotation);
  next_tetromino_number = generateRandomNumber();
  next_tetromino = getTetrominoCoordinates(next_tetromino_number, tetromino_rotation);

  display.setCursor(POINTS_TEXT_X_COORDINATES, POINTS_TEXT_Y_COORDINATES);
  display.setTextColor(WHITE);
  display.print("POINTS");
  display.setCursor(POINTS_X_COORDINATES, POINTS_Y_COORDINATES);
  display.print(points);
  drawTetromino(NEXT_TETROMINO_PREVIEW_X_COORDINATES, NEXT_TETROMINO_PREVIEW_Y_COORDINATES, block_preview_size, next_tetromino.current);
  drawTetromino(map_x, map_y, block_size, tetromino.current);
}

void loop() {

  // GAME OVER
  if (is_game_over) {
    display.clearDisplay();
    display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, GAME_CONTAINER_WIDTH, GAME_CONTAINER_HEIGHT, WHITE);
    display.setCursor(GAME_OVER_TEXT_X, GAME_OVER_TEXT_Y);
    display.print(GAME_OVER_TEXT);
    display.display();
    for(;;);
  }

  // WINNER
  if (points >= POINTS_TO_WIN) {
    display.clearDisplay();
    display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, GAME_CONTAINER_WIDTH, GAME_CONTAINER_HEIGHT, WHITE);
    display.setCursor(WINNING_TEXT_X, WINNING_TEXT_Y);
    display.print(WINNING_TEXT);
    display.display();
    for(;;);
  }
  
  
  // ROTATION
  if (digitalRead(BUTTON_ROTATION)) {
    delay(150);
    tetromino = rotateTetromino(&map_x, map_y, tetromino, tetromino_number, &tetromino_rotation, game_map);
    old_map_x = map_x;
    last_time = millis();
  }

  // GO DOWN AUTOMATICALLY
  if ((millis() - last_time) >= PAUSE) {
    if (map_y > 0) {
      cancelTetromino(old_map_x, old_map_y, block_size, tetromino.current);
      old_map_x = map_x;
      old_map_y = map_y;
    }
    // handle stop tetromino  
    drawTetromino(map_x, map_y, block_size, tetromino.current);

    // update time
    last_time = millis();

    bool go_down = canGoFurtherDown(map_x, map_y, tetromino.current, game_map);
    if (!go_down) {
      printOnMap(map_x, map_y, tetromino.current, game_map);
      is_game_over = checkGameOver(game_map, next_tetromino_number);
      if (is_game_over) return;

      map_x = START_MAP_X;
      map_y = START_MAP_Y;
      old_map_x = map_x;
      old_map_y = map_y;

      // cancel preview
      cancelTetromino(NEXT_TETROMINO_PREVIEW_X_COORDINATES, NEXT_TETROMINO_PREVIEW_Y_COORDINATES, block_preview_size, next_tetromino.current);

      // tetromino new value
      tetromino_number = next_tetromino_number;
      tetromino = next_tetromino;
      tetromino_rotation = 0;

      // create next tetromino
      next_tetromino_number = generateRandomNumber();
      next_tetromino = getTetrominoCoordinates(next_tetromino_number, tetromino_rotation);
      drawTetromino(NEXT_TETROMINO_PREVIEW_X_COORDINATES, NEXT_TETROMINO_PREVIEW_Y_COORDINATES, block_preview_size, next_tetromino.current);
    } else {
      map_y++;
    }    
  }

  // BUTTON DOWN
  if (digitalRead(BUTTON_DOWN)) {
    delay(150);
    if (map_y > 0) {
      cancelTetromino(old_map_x, old_map_y, block_size, tetromino.current);
      old_map_x = map_x;
      old_map_y = map_y;
    } 
    drawTetromino(map_x, map_y, block_size, tetromino.current);

    // update time
    last_time = millis();

    bool go_down = canGoFurtherDown(map_x, map_y, tetromino.current, game_map);
    if (!go_down) {
      printOnMap(map_x, map_y, tetromino.current, game_map);
      is_game_over = checkGameOver(game_map, next_tetromino_number);
      if (is_game_over) return;

      map_x = START_MAP_X;
      map_y = START_MAP_Y;
      old_map_x = map_x;
      old_map_y = map_y;

      // cancel preview
      cancelTetromino(NEXT_TETROMINO_PREVIEW_X_COORDINATES, NEXT_TETROMINO_PREVIEW_Y_COORDINATES, block_preview_size, next_tetromino.current);

      // tetromino new value
      tetromino_number = next_tetromino_number;
      tetromino = next_tetromino;
      tetromino_rotation = 0;

      // create next tetromino
      next_tetromino_number = generateRandomNumber();
      next_tetromino = getTetrominoCoordinates(next_tetromino_number, tetromino_rotation);
      drawTetromino(NEXT_TETROMINO_PREVIEW_X_COORDINATES, NEXT_TETROMINO_PREVIEW_Y_COORDINATES, block_preview_size, next_tetromino.current);
    } else {
      map_y++;
    }
  }

  // BUTTON LEFT
  if (digitalRead(BUTTON_LEFT)) {
    delay(150);    
    bool go_left = canGoLeft(map_x, map_y, tetromino.current, game_map);    
    if (!go_left) return;
    cancelTetromino(old_map_x, old_map_y, block_size, tetromino.current);
    map_x--;
    drawTetromino(map_x, map_y, block_size, tetromino.current);
    old_map_x = map_x;
    old_map_y = map_y;
  }

  // BUTTON RIGHT
  if (digitalRead(BUTTON_RIGHT)) {
    delay(150);
    bool go_right = canGoRight(map_x, map_y, tetromino.current, game_map);
    if (!go_right) return;
    cancelTetromino(old_map_x, old_map_y, block_size, tetromino.current);
    map_x++;
    drawTetromino(map_x, map_y, block_size, tetromino.current);
    old_map_x = map_x;
    old_map_y = map_y;
  }

  // CHECK FOR COMPLETE ROWS
  checkCompleteRow(game_map);  
  
}