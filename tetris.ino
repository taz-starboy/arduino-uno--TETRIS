#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// **** VARIABLES
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// defining a grid 20x10 blocks (120x60 pixels)
#define VERTICAL_MARGIN_PIXELS 4
#define HORIZONTAL_MARGIN_PIXELS 20

#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 4
#define BUTTON_ROTATION 5
#define BUZZER 13

#define PAUSE 1000

#define SIZE 6

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint32_t last_time = millis();
uint8_t map_x = 0;
uint8_t map_y = 0;
uint8_t old_map_x = map_x;  // 0
uint8_t old_map_y = map_y;  // 0

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
const uint8_t TTM_SQUARE[8] PROGMEM = {0, 0, 1, 0, 0, 1, 1, 1};
const uint8_t TTM_S[2][8] PROGMEM = {
  {0, 0, 0, 1, 1, 1, 1, 2},
  {1, 1, 2, 1, 0, 2, 1, 2}
};


// **** START
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
  

  display.clearDisplay();
  display.setRotation(1); // set display to vertical orientation  

  display.drawRect(1, 3, 62, 122, WHITE); // draw game table

  randomSeed(analogRead(A0)); // initialize random seed
  tetraminoe_number = generateRandomNumber(); // move to loop to avoid generating 0 as first
  Serial.print("random number: ");
  Serial.println(tetraminoe_number);

  generateTetraminoe(tetraminoe_number);
}

void loop() {
  /*
    occorre passare tutte la variabili alle funzioni in modo che in futuro per separare la logica sia piu facile
  */
  // button varation pressed
  if (digitalRead(BUTTON_ROTATION)) {
    delay(150);   
    rotateTetraminoe(map_x, map_y, tetraminoe_number, &tetraminoe_rotation);
    // update time // VALUTARE SE METTERLO DIRETTAMENTE NELLA FUNZIONE DRAWTETRAMINOE
    last_time = millis();
  }


  if ((millis() - last_time) >= PAUSE) { // o il pulsante giu e stato premuto
    generateTetraminoe(tetraminoe_number);
    // qui e necessario sapere in che variante siamo
    // che tipo di tetramino e
    // calcolare se puo ancora andare giu  
  }
  // button down pressed
  if (digitalRead(BUTTON_DOWN)) {
    delay(150);
    generateTetraminoe(tetraminoe_number);
  }
  
  // button left pressed
  // if (digitalRead(BUTTON_LEFT)) {
  //   delay(150);
  //   bool go_left = canGoLeft(map_x, map_y, tetraminoe_number); // qui non so che tipo di tetramino si tratta
  //   generateTetraminoe(tetraminoe_number);
  // }
  // button right pressed
  // if (digitalRead(BUTTON_RIGHT)) {
  //   delay(150);
  //   generateTetraminoe(tetraminoe_number);
  // }  
}


// **** FUNCTIONS
void rotateTetraminoe(uint8_t map_x, uint8_t map_y, uint8_t tetraminoe_number, uint8_t *tetraminoe_rotation) { // rotation occurs on same position
  switch (tetraminoe_number) {
    case 0: // TTM_SQUARE
      // no need rotation
      return;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5: // s right
      uint8_t new_rotation = *tetraminoe_rotation > 0 ? 0 : 1;
      bool okToRotate = validateRotation(map_x, map_y, TTM_S[new_rotation]);
      if (okToRotate) {
        //cancelTetraminoe(map_x, map_y, TTM_S[*tetraminoe_rotation]);
        cancelTetraminoe(old_map_x, old_map_y, TTM_S[*tetraminoe_rotation]);
        *tetraminoe_rotation = new_rotation;
        drawTetraminoe(old_map_x, old_map_y, TTM_S[new_rotation]);
      }
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      Serial.println(F("No match for a tetraminoe, impossible determine rotation."));
  }
  display.display(); 
}
bool validateRotation(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t x_value = map_x + pgm_read_byte(&tetraminoe_coordinates[i]);
    uint8_t y_value = map_y + pgm_read_byte(&tetraminoe_coordinates[i + 1]);
    if (x_value < 0) return false;
    if (x_value > 9) return false;
    if (game_map[y_value][x_value] == 1) return false;
  }
  return true;
}
void generateTetraminoe(uint8_t number) {
  switch (number) {
    case 0:
      updateTetraminoe(map_x, map_y, TTM_SQUARE);
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      updateTetraminoe(map_x, map_y, TTM_S[tetraminoe_rotation]);
      break;
    case 6:
      break;
    case 7:
      break;
    default:
      Serial.println(F("No match for a tetraminoe."));
  }
  display.display();
}

void updateTetraminoe(uint8_t _X_, uint8_t _Y_, const uint8_t tetraminoe_coordinates[8]) { 
  // cancel last TTM_SQUARE
  if (_Y_ > 0) {
    cancelTetraminoe(old_map_x, old_map_y, tetraminoe_coordinates);
    old_map_x = _X_;
    old_map_y = _Y_;
  } 

  // conversion to real coordinates in pixels  
  drawTetraminoe(_X_, _Y_, tetraminoe_coordinates);
  
  // update time
  last_time = millis(); 
  
  // handle stop tetraminoe
  bool go_down = canGoDown(_X_, _Y_, tetraminoe_coordinates);
  if (!go_down) {
    Serial.println("reach last step down");
    printOnMap(_X_, _Y_, tetraminoe_coordinates);
    
    map_x = 0;
    map_y = 0;
    old_map_x = map_x;
    old_map_y = map_y;

    tetraminoe_number = 5;
    tetraminoe_rotation = 0;

    return;
  }

  // increment axis y on game_map
  map_y++;  
}
// bool canGoLeft(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
//   uint8_t x = HORIZONTAL_MARGIN_PIXELS + _X_ * SIZE;
//   uint8_t y = VERTICAL_MARGIN_PIXELS + _Y_ * SIZE;
  
//   for (uint8_t i = 0; i < 8; i += 2) {
//     uint8_t x_value = x + pgm_read_byte(&tetraminoe_coordinates[i);
//     uint8_t y_value = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]);
//     if (x_value == 0) return false;
//     if (game_map[y_value][x_value - 1] == 1) return false;
//   }

//   tetraminoe_rotation++;
//   return true;
// }
bool canGoDown(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t x_value = x + pgm_read_byte(&tetraminoe_coordinates[i]);
    uint8_t y_value = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]);
    if (y_value == 19) return false;
    if (game_map[y_value + 1][x_value] == 1) return false;
  }
  return true;
}
void printOnMap(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t var_x = x + pgm_read_byte(&tetraminoe_coordinates[i]);
    uint8_t var_y = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]);    
    game_map[var_y][var_x] = 1;
  }
}
void drawTetraminoe(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * SIZE;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * SIZE;
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + pgm_read_byte(&tetraminoe_coordinates[i]) * SIZE;
    uint8_t start_y = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]) * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, WHITE);
  }
}
void cancelTetraminoe(uint8_t map_x, uint8_t map_y, const uint8_t tetraminoe_coordinates[8]) {
  // conversion to real coordinates in pixels
  uint8_t x = HORIZONTAL_MARGIN_PIXELS + map_x * SIZE;
  uint8_t y = VERTICAL_MARGIN_PIXELS + map_y * SIZE;  
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + pgm_read_byte(&tetraminoe_coordinates[i]) * SIZE;
    uint8_t start_y = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]) * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, BLACK);
  }
}
uint8_t generateRandomNumber() {
  return random(0, 7);
}