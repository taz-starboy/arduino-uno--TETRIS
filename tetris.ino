#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// **** VARIABLES
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// defining a grid 20x10 blocks (6*20) (6*10) (120x60 pixels)
// tetromino starting coordinates in pixels
#define HORIZONTAL_MARGIN_PIXELS 2
#define VERTICAL_MARGIN_PIXELS 4

#define POINTS_TEXT_X_COORDINATES 5
#define POINTS_TEXT_Y_COORDINATES 6
#define POINTS_X_COORDINATES 5
#define POINTS_Y_COORDINATES 17
uint8_t points = 0;

#define NEXT_TETROMINO_PREVIEW_X_COORDINATES 11
#define NEXT_TETROMINO_PREVIEW_Y_COORDINATES 1


#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4
#define BUTTON_ROTATION 5
#define BUZZER 13

#define PAUSE 1000
#define POINTS_TO_WIN 200

#define START_MAP_X 4
#define START_MAP_Y 4

uint8_t block_size = 6;
uint8_t block_preview_size = 4;

uint8_t map_x = START_MAP_X;
uint8_t map_y = START_MAP_Y;
uint8_t old_map_x = map_x;  // 0
uint8_t old_map_y = map_y;  // 0

struct Tetromino {
  uint8_t current[8];
  uint8_t max_rotations;
};
Tetromino tetromino;
uint8_t tetromino_number;
Tetromino next_tetromino;
uint8_t next_tetromino_number;
uint8_t tetromino_rotation = 0;

bool is_game_over = false;

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
const uint8_t SQUARE_COORDINATES[8] PROGMEM = {0, 0, 1, 0, 0, 1, 1, 1};
const uint8_t S_COORDINATES[2][8] PROGMEM = {
  {0, 0, 0, 1, 1, 1, 1, 2},
  {1, 1, 2, 1, 0, 2, 1, 2}
};
const uint8_t Z_COORDINATES[2][8] PROGMEM = {
  {1, 0, 0, 1, 1, 1, 0, 2},
  {0, 1, 1, 1, 1, 2, 2, 2}
};
const uint8_t L_COORDINATES[4][8] PROGMEM = {
  {0, 0, 0, 1, 0, 2, 1, 2},
  {0, 2, 1, 2, 2, 2, 2, 1},
  {0, 0, 1, 0, 1, 1, 1, 2},
  {0, 1, 1, 1, 2, 1, 0, 2}
};
const uint8_t J_COORDINATES[4][8] PROGMEM = {
  {1, 0, 1, 1, 1, 2, 0, 2},
  {0, 1, 1, 1, 2, 1, 2, 2},
  {0, 0, 1, 0, 0, 1, 0, 2},
  {0, 1, 0, 2, 1, 2, 2, 2}
};
const uint8_t T_COORDINATES[4][8] PROGMEM = {
  {1, 1, 0, 2, 1, 2, 2, 2},
  {0, 1, 1, 0, 1, 1, 1, 2},
  {0, 0, 1, 0, 2, 0, 1, 1},
  {0, 0, 0, 1, 1, 1, 0, 2}
};
const uint8_t I_COORDINATES[2][8] PROGMEM = {
  {0, 0, 0, 1, 0, 2, 0, 3},
  {0, 3, 1, 3, 2, 3, 3, 3}
};

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
  display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, 62, 122, WHITE); 
  display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, 62, START_MAP_Y * block_size + 1, WHITE);

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
    display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, 62, 122, WHITE);
    display.setCursor(5, 42);
    display.print("GAME OVER");
    display.display();
    for(;;);
  }

  // WINNER
  if (points >= POINTS_TO_WIN) {
    display.clearDisplay();
    display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, 62, 122, WHITE);
    display.setCursor(11, 42);
    display.print("YOU WIN");
    display.display();
    for(;;);
  }
  
  
  // ROTATION
  if (digitalRead(BUTTON_ROTATION)) {
    delay(150);
    tetromino = rotateTetromino(&map_x, map_y, tetromino, tetromino_number, &tetromino_rotation);
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

    bool go_down = canGoFurtherDown(map_x, map_y, tetromino.current);
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

    bool go_down = canGoFurtherDown(map_x, map_y, tetromino.current);
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


// **** FUNCTIONS ****
Tetromino getTetrominoCoordinates(uint8_t tetromino_number, uint8_t tetromino_rotation) { // questa funzione ritorna solo l'array con la giusta rotazione
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

Tetromino rotateTetromino(uint8_t *map_x, uint8_t map_y, Tetromino tetromino, uint8_t tetromino_number, uint8_t *tetromino_rotation) { // rotation occurs on same position
  
  uint8_t new_rotation = *tetromino_rotation;
  new_rotation++;

  if (new_rotation >= tetromino.max_rotations) new_rotation = 0;

  Tetromino turned_tetromino = getTetrominoCoordinates(tetromino_number, new_rotation);
  bool okToRotate = validateRotation(map_x, map_y, turned_tetromino.current);
  if (okToRotate) {
    cancelTetromino(old_map_x, old_map_y, block_size, tetromino.current);
    drawTetromino(*map_x, old_map_y, block_size, turned_tetromino.current);
    *tetromino_rotation = new_rotation;    
    return turned_tetromino;
  }
  return tetromino; 
}
bool validateRotation(uint8_t *map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    int8_t x_value = (*map_x) + tetromino_coordinates[i];
    uint8_t y_value = map_y + tetromino_coordinates[i + 1];
    if (x_value < 0) return false;
    if (x_value > 9) {
      return validateRotation(&(--(*map_x)), map_y, tetromino_coordinates);
    }
    if (game_map[y_value][x_value] == 1) return false;
  }
  return true;
}
bool canGoFurtherDown(uint8_t x, uint8_t y, const uint8_t tetromino_coordinates[8]) {
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
void cancelNextTetromino(uint8_t map_x, uint8_t map_y, const uint8_t tetromino_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * 4;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * 4;  
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + tetromino_coordinates[i] * 4;
    uint8_t start_y = y + tetromino_coordinates[i + 1] * 4;
    display.drawRect(start_x, start_y, 4, 4, BLACK);
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