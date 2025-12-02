#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// **** VARIABLES
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// defining a grid 20x10 blocks (6*20) (6*10) (120x60 pixels)
// tetraminoe starting coordinates in pixels
#define HORIZONTAL_MARGIN_PIXELS 2
#define VERTICAL_MARGIN_PIXELS 4

#define POINTS_TEXT_X_COORDINATES 5
#define POINTS_TEXT_Y_COORDINATES 6
#define POINTS_X_COORDINATES 5
#define POINTS_Y_COORDINATES 17
uint8_t points = 0;

#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4
#define BUTTON_ROTATION 5
#define BUZZER 13

#define PAUSE 1000

#define SIZE 6

#define START_MAP_X 4
#define START_MAP_Y 4

uint8_t map_x = START_MAP_X;
uint8_t map_y = START_MAP_Y;
uint8_t old_map_x = map_x;  // 0
uint8_t old_map_y = map_y;  // 0

struct Tetraminoe {
  uint8_t current[8];
  uint8_t max_rotations;
};
Tetraminoe tetraminoe;
uint8_t tetraminoe_number;
uint8_t tetraminoe_rotation = 0;

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
  //{2, 0, 1, 1, 2, 1, 2, 2},
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

  display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, 62, 122, WHITE); // draw game table
  display.drawRect(HORIZONTAL_MARGIN_PIXELS - 1, VERTICAL_MARGIN_PIXELS - 1, 62, START_MAP_Y * SIZE + 1, WHITE); // draw game table

  randomSeed(analogRead(A0)); // initialize random seed
  tetraminoe_number = generateRandomNumber(); // move to loop to avoid generating 0 as first
  tetraminoe = getTetraminoeCoordinates(tetraminoe_number, tetraminoe_rotation);

  display.setCursor(POINTS_TEXT_X_COORDINATES, POINTS_TEXT_Y_COORDINATES);
  display.setTextColor(WHITE);
  display.print("POINTS");
  display.setCursor(5, 17);
  display.print(points);
  drawTetraminoe(map_x, map_y, tetraminoe.current);
}

void loop() {
  
  // ROTATION
  if (digitalRead(BUTTON_ROTATION)) {
    delay(150);
    tetraminoe = rotateTetraminoe(map_x, map_y, tetraminoe, tetraminoe_number, &tetraminoe_rotation);
    last_time = millis();
  }

  // GO DOWN AUTOMATICALLY
  if ((millis() - last_time) >= PAUSE) { // o il pulsante giu e stato premuto
    if (map_y > 0) {
      cancelTetraminoe(old_map_x, old_map_y, tetraminoe.current);
      old_map_x = map_x;
      old_map_y = map_y;
    }
    // handle stop tetraminoe  
    drawTetraminoe(map_x, map_y, tetraminoe.current);
    
    // update time
    last_time = millis();
    
    bool go_down = canGoFurtherDown(map_x, map_y, tetraminoe.current);
    if (!go_down) {
      printOnMap(map_x, map_y, tetraminoe.current, game_map);
      
      map_x = START_MAP_X;
      map_y = START_MAP_Y;
      old_map_x = map_x;
      old_map_y = map_y;

      tetraminoe_number = generateRandomNumber();;
      tetraminoe_rotation = 0;

      tetraminoe = getTetraminoeCoordinates(tetraminoe_number, tetraminoe_rotation);
      //return;
    } else {
      map_y++;
    }    
  }

  // BUTTON DOWN
  if (digitalRead(BUTTON_DOWN)) {
    delay(150);
    if (map_y > 0) {
      cancelTetraminoe(old_map_x, old_map_y, tetraminoe.current);
      old_map_x = map_x;
      old_map_y = map_y;
    } 
    drawTetraminoe(map_x, map_y, tetraminoe.current);
    
    // update time
    last_time = millis();
    
    bool go_down = canGoFurtherDown(map_x, map_y, tetraminoe.current);
    if (!go_down) {
      printOnMap(map_x, map_y, tetraminoe.current, game_map);
      
      map_x = START_MAP_X;
      map_y = START_MAP_Y;
      old_map_x = map_x;
      old_map_y = map_y;

      tetraminoe_number = generateRandomNumber();
      tetraminoe_rotation = 0;

      tetraminoe = getTetraminoeCoordinates(tetraminoe_number, tetraminoe_rotation);
    } else {
      map_y++;
    }
  }
  
  // BUTTON LEFT
  if (digitalRead(BUTTON_LEFT)) {
    delay(150);    
    bool go_left = canGoLeft(map_x, map_y, tetraminoe.current, game_map);    
    if (!go_left) return;
    cancelTetraminoe(old_map_x, old_map_y, tetraminoe.current);
    map_x--;
    drawTetraminoe(map_x, map_y, tetraminoe.current);
    old_map_x = map_x;
    old_map_y = map_y;
  }

  // BUTTON RIGHT
  if (digitalRead(BUTTON_RIGHT)) {
    delay(150);
    bool go_right = canGoRight(map_x, map_y, tetraminoe.current, game_map);
    if (!go_right) return;
    cancelTetraminoe(old_map_x, old_map_y, tetraminoe.current);
    map_x++;
    drawTetraminoe(map_x, map_y, tetraminoe.current);
    old_map_x = map_x;
    old_map_y = map_y;
  }

  // CHECK FOR COMPLETE ROWS
  checkCompleteRow(game_map);
}


// **** FUNCTIONS ****
Tetraminoe getTetraminoeCoordinates(uint8_t tetraminoe_number, uint8_t tetraminoe_rotation) { // questa funzione ritorna solo l'array con la giusta rotazione
  Tetraminoe tetraminoe;

  switch (tetraminoe_number) {
    case 0: // square      
      memcpy_P(tetraminoe.current, SQUARE_COORDINATES, 8);
      tetraminoe.max_rotations = 0;
      break;
    case 1: // s
      memcpy_P(tetraminoe.current, S_COORDINATES[tetraminoe_rotation], 8);
      tetraminoe.max_rotations = 2;
      break;
    case 2:
      memcpy_P(tetraminoe.current, Z_COORDINATES[tetraminoe_rotation], 8);
      tetraminoe.max_rotations = 2;
      break;
    case 3:
      memcpy_P(tetraminoe.current, L_COORDINATES[tetraminoe_rotation], 8);
      tetraminoe.max_rotations = 4;
      break;
    case 4:
      memcpy_P(tetraminoe.current, J_COORDINATES[tetraminoe_rotation], 8);
      tetraminoe.max_rotations = 4;
      break;
    case 5:
      memcpy_P(tetraminoe.current, T_COORDINATES[tetraminoe_rotation], 8);
      tetraminoe.max_rotations = 4;
      break;
    case 6:
      memcpy_P(tetraminoe.current, I_COORDINATES[tetraminoe_rotation], 8);
      tetraminoe.max_rotations = 2;
      break;
    default:
      Serial.println(F("No match for a tetraminoe."));
  }
  return tetraminoe;
}

Tetraminoe rotateTetraminoe(uint8_t map_x, uint8_t map_y, Tetraminoe tetraminoe, uint8_t tetraminoe_number, uint8_t *tetraminoe_rotation) { // rotation occurs on same position 
  uint8_t new_rotation = *tetraminoe_rotation;
  new_rotation++;

  if (new_rotation >= tetraminoe.max_rotations) new_rotation = 0;

  Tetraminoe turned_tetraminoe = getTetraminoeCoordinates(tetraminoe_number, new_rotation);
  bool okToRotate = validateRotation(map_x, map_y, turned_tetraminoe.current);
  if (okToRotate) {
    cancelTetraminoe(old_map_x, old_map_y, tetraminoe.current);
    drawTetraminoe(old_map_x, old_map_y, turned_tetraminoe.current);
    *tetraminoe_rotation = new_rotation;    
    return turned_tetraminoe;
  }
  return tetraminoe; 
}
bool validateRotation(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    int8_t x_value = map_x + tetraminoe_coordinates[i];
    uint8_t y_value = map_y + tetraminoe_coordinates[i + 1];
    if (x_value < 0) return false;
    if (x_value > 9) return false;
    if (game_map[y_value][x_value] == 1) return false;
  }
  return true;
}
bool canGoFurtherDown(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t x_value = x + tetraminoe_coordinates[i];
    uint8_t y_value = y + tetraminoe_coordinates[i + 1];
    if (y_value == 19) return false;
    if (game_map[y_value + 1][x_value] == 1) return false;
  }
  return true;
}
bool canGoLeft(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8], uint8_t game_map[20][10]) {
  if (map_x < 1) return false;
  for (uint8_t i = 0; i < 8; i += 2) {
    int8_t x_value = map_x + tetraminoe_coordinates[i];
    if (x_value < 1) return false;
    uint8_t y_value = map_y + tetraminoe_coordinates[i + 1];    
    if (game_map[y_value][x_value - 1] == 1) return false;
  }
  return true;
}
bool canGoRight(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8], uint8_t game_map[20][10]) {
  if (map_x > 8) return false;
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t x_value = map_x + tetraminoe_coordinates[i];
    if (x_value > 8) return false;
    uint8_t y_value = map_y + tetraminoe_coordinates[i + 1];    
    if (game_map[y_value][x_value + 1] == 1) return false;
  }
  return true;
}
void printOnMap(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8], uint8_t game_map[20][10]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t var_x = x + tetraminoe_coordinates[i];
    uint8_t var_y = y + tetraminoe_coordinates[i + 1]; 
    game_map[var_y][var_x] = 1;
  }
}
void drawTetraminoe(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * SIZE;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * SIZE;
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + tetraminoe_coordinates[i] * SIZE;
    uint8_t start_y = y + tetraminoe_coordinates[i + 1] * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, WHITE);
  }
  display.display();
}
void cancelTetraminoe(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * SIZE;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * SIZE;  
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + tetraminoe_coordinates[i] * SIZE;
    uint8_t start_y = y + tetraminoe_coordinates[i + 1] * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, BLACK);
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
  uint8_t y = VERTICAL_MARGIN_PIXELS + row_number * SIZE;
  for (uint8_t i = 0; i < 10; i++) {
    uint8_t x = HORIZONTAL_MARGIN_PIXELS + i * SIZE;
    if (row[i] == 1) {      
      display.drawRect(x, y, SIZE, SIZE, WHITE);
      continue;
    }
    display.drawRect(x, y, SIZE, SIZE, BLACK);
  }
}
void updatePoints(uint8_t x_points, uint8_t y_points, uint8_t *points) {
  (*points) += 10; 
  display.fillRect(x_points, y_points, 35, 7, BLACK);
  display.setCursor(x_points, y_points);
  display.print(*points);  
}
uint8_t generateRandomNumber() {
  return random(0, 7);
}