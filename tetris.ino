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
#define VERTICAL_MARGIN_PIXELS 4
#define HORIZONTAL_MARGIN_PIXELS 2

#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4
#define BUTTON_ROTATION 5
#define BUZZER 13

#define PAUSE 1000

#define SIZE 6

#define START_X 4
#define START_Y 0

int8_t map_x = START_X;
int8_t map_y = START_Y;
int8_t old_map_x = map_x;  // 0
int8_t old_map_y = map_y;  // 0

struct Tetraminoe {
  int8_t current[8];
  int8_t max_rotations;
};
Tetraminoe tetraminoe;
int8_t tetraminoe_number;
int8_t tetraminoe_rotation = 0;

int8_t game_map[20][10] = {
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
const int8_t SQUARE_COORDINATES[8] PROGMEM = {0, 0, 1, 0, 0, 1, 1, 1};
const int8_t S_COORDINATES[2][8] PROGMEM = {
  {0, 0, 0, 1, 1, 1, 1, 2},
  {1, 1, 2, 1, 0, 2, 1, 2}
};
const int8_t Z_COORDINATES[2][8] PROGMEM = {
  {1, 0, 0, 1, 1, 1, 0, 2},
  {0, 1, 1, 1, 1, 2, 2, 2}
};
const int8_t L_COORDINATES[4][8] PROGMEM = {
  {0, 0, 0, 1, 0, 2, 1, 2},
  {0, 2, 1, 2, 2, 2, 2, 1},
  {0, 0, 1, 0, 1, 1, 1, 2},
  {0, 1, 1, 1, 2, 1, 0, 2}
};
const int8_t J_COORDINATES[4][8] PROGMEM = {
  {1, 0, 1, 1, 1, 2, 0, 2},
  {0, 1, 1, 1, 1, 2, 2, 2},
  {0, 0, 1, 0, 0, 1, 0, 2},
  {0, 1, 0, 2, 1, 2, 2, 2}
};
const int8_t T_COORDINATES[4][8] PROGMEM = {
  {1, 1, 0, 2, 1, 2, 2, 2},
  {2, 0, 1, 1, 2, 1, 2, 2},
  {0, 0, 1, 0, 2, 0, 1, 1},
  {0, 0, 0, 1, 1, 1, 0, 2}
};
const int8_t I_COORDINATES[2][8] PROGMEM = {
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

  display.drawRect(1, 3, 62, 122, WHITE); // draw game table

  randomSeed(analogRead(A0)); // initialize random seed
  tetraminoe_number = generateRandomNumber(); // move to loop to avoid generating 0 as first
  tetraminoe = getTetraminoeCoordinates(tetraminoe_number, tetraminoe_rotation);
  // Serial.print("random number: ");
  // Serial.println(tetraminoe_number);

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
    
    // for (int8_t i = 0; i < 8; i++) {
    //   Serial.print(tetraminoe.current[i]);      
    // }
    // Serial.print("\n");

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
      Serial.println(F("reach last step down"));
      printOnMap(map_x, map_y, tetraminoe.current, game_map);
      
      map_x = START_X;
      map_y = START_Y;
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
      Serial.println(F("reach last step down"));
      printOnMap(map_x, map_y, tetraminoe.current, game_map);
      
      map_x = START_X;
      map_y = START_Y;
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
}


// **** FUNCTIONS ****
Tetraminoe getTetraminoeCoordinates(int8_t tetraminoe_number, int8_t tetraminoe_rotation) { // questa funzione ritorna solo l'array con la giusta rotazione
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

Tetraminoe rotateTetraminoe(int8_t map_x, int8_t map_y, Tetraminoe tetraminoe, int8_t tetraminoe_number, int8_t *tetraminoe_rotation) { // rotation occurs on same position 
  int8_t new_rotation = *tetraminoe_rotation;
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
bool validateRotation(int8_t map_x, int8_t map_y, const int8_t tetraminoe_coordinates[8]) {
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t x_value = map_x + tetraminoe_coordinates[i];
    int8_t y_value = map_y + tetraminoe_coordinates[i + 1];
    if (x_value < 0) return false;
    if (x_value > 9) return false;
    if (game_map[y_value][x_value] == 1) return false;
  }
  return true;
}
bool canGoFurtherDown(int8_t x, int8_t y, const int8_t tetraminoe_coordinates[8]) {
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t x_value = x + tetraminoe_coordinates[i];
    int8_t y_value = y + tetraminoe_coordinates[i + 1];
    if (y_value == 19) return false;
    if (game_map[y_value + 1][x_value] == 1) return false;
  }
  return true;
}
bool canGoLeft(int8_t map_x, int8_t map_y, const int8_t tetraminoe_coordinates[8], int8_t game_map[20][10]) {
  if (map_x < 1) return false;
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t x_value = map_x + tetraminoe_coordinates[i];
    if (x_value < 1) return false;
    int8_t y_value = map_y + tetraminoe_coordinates[i + 1];    
    if (game_map[y_value][x_value - 1] == 1) return false;
  }
  return true;
}
bool canGoRight(int8_t map_x, int8_t map_y, const int8_t tetraminoe_coordinates[8], int8_t game_map[20][10]) {
  if (map_x > 8) return false;
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t x_value = map_x + tetraminoe_coordinates[i];
    if (x_value > 8) return false;
    int8_t y_value = map_y + tetraminoe_coordinates[i + 1];    
    if (game_map[y_value][x_value + 1] == 1) return false;
  }
  return true;
}
void printOnMap(int8_t x, int8_t y, const int8_t tetraminoe_coordinates[8], int8_t game_map[20][10]) {
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t var_x = x + tetraminoe_coordinates[i];
    int8_t var_y = y + tetraminoe_coordinates[i + 1]; 
    game_map[var_y][var_x] = 1;
  }
  for (int8_t i = 0; i < 20; i++) {    
    for (int8_t j = 0; j < 10; j++) {    
      Serial.print(game_map[i][j]);
    }
    Serial.print("\n");
  }
}
void drawTetraminoe(int8_t map_x, int8_t map_y, const int8_t tetraminoe_coordinates[8]) {
  // conversion to real coordinates in pixels
  int8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * SIZE;
  int8_t y = VERTICAL_MARGIN_PIXELS + map_y * SIZE;
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t start_x = x + tetraminoe_coordinates[i] * SIZE;
    int8_t start_y = y + tetraminoe_coordinates[i + 1] * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, WHITE);
  }
  display.display();
}
void cancelTetraminoe(int8_t map_x, int8_t map_y, const int8_t tetraminoe_coordinates[8]) {
  // conversion to real coordinates in pixels
  int8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * SIZE;
  int8_t y = VERTICAL_MARGIN_PIXELS + map_y * SIZE;  
  for (int8_t i = 0; i < 8; i += 2) {
    int8_t start_x = x + tetraminoe_coordinates[i] * SIZE;
    int8_t start_y = y + tetraminoe_coordinates[i + 1] * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, BLACK);
  }
}
int8_t generateRandomNumber() {
  return random(0, 7);
}