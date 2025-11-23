#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// **** VARIABLES
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON_DOWN 2
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
uint8_t tetraminoe_variation = 0;

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
const uint8_t SQUARE[8] PROGMEM = {0,0,1,0,0,1,1,1};
const uint8_t ES[4][8] PROGMEM = {
  {0, 0, 0, 1, 1, 1, 1, 2},
  {1, 0, 2, 0, 0, 1, 1, 1},
  {0, 0, 0, 1, 1, 1, 2, 2},
  {0, 2, 1, 2, 1, 2, 2, 1}
};


// **** START
void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  pinMode(BUTTON_DOWN, INPUT);
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
  
  if ((millis() - last_time) >= PAUSE) {
    generateTetraminoe(tetraminoe_number);   
  }

  // button down pressed
  if (digitalRead(BUTTON_DOWN)) {
    delay(150);
    generateTetraminoe(tetraminoe_number);
  }
}


// **** FUNCTIONS
void generateTetraminoe(uint8_t number) {
  switch (number) {
    case 0:
      drawSquare(map_x, map_y);
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
      drawEs(map_x, map_y);
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
void incrementY() {
  map_y = map_y < 18 ? map_y + 1 : 0;
}
uint8_t generateRandomNumber() {
  return random(0, 7);
}
void drawSquare(uint8_t _X_, uint8_t _Y_) { 
  
  // cancel last square
  if (_Y_ > 0) {
    //uint8_t old_Y_ = _Y_ - 1;    
    // conversion to real coordinates in pixels
    uint8_t x = 2 + old_map_x * SIZE;
    uint8_t y = 4 + old_map_y * SIZE;
    cancelBlock(x, y, SQUARE);
    old_map_x = _X_;
    old_map_y = _Y_;
  } 

  // conversion to real coordinates in pixels
  uint8_t x = 2 + _X_ * SIZE;
  uint8_t y = 4 + _Y_ * SIZE;

  // draw new square
  drawBlock(x, y, SQUARE);
  
  // update time
  last_time = millis(); 
  
  // handle stop tetraminoe
  if (_Y_ > 17 || game_map[_Y_ + 2][_X_]) {
    Serial.println("fine corsa");
    printOnMap(_X_, _Y_, SQUARE);
    
    map_x = 0;
    map_y = 0;
    old_map_x = map_x;
    old_map_y = map_y;

    tetraminoe_number = 5;

    return;
  }

  // increment axis y on game_map
  map_y++;  
}
void printOnMap(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t var_x = x + pgm_read_byte(&tetraminoe_coordinates[i]);
    Serial.print("x in map: ");
    Serial.println(var_x);
    uint8_t var_y = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]);
    Serial.print("y in map: ");
    Serial.println(var_y);
    
    game_map[var_y][var_x] = 1;
  }
}
void drawEs(uint8_t _X_, uint8_t _Y_) {
  // cancel last es
  if (_Y_ > 0) {
    uint8_t old_Y_ = _Y_ - 1;    
    // conversion to real coordinates in pixels
    uint8_t x = 2 + _X_ * SIZE;
    uint8_t y = 4 + old_Y_ * SIZE;
    cancelBlock(x, y, ES[0]); 
  }

  // conversion to real coordinates in pixels
  uint8_t x = 2 + _X_ * SIZE;
  uint8_t y = 4 + _Y_ * SIZE;

  // draw new square
  drawBlock(x, y, ES[0]);

  // update time
  last_time = millis(); 
  
  // handle stop tetraminoe
  if (_Y_ > 16 || game_map[_Y_ + 2][_X_]) {
    Serial.println("fine corsa");
    game_map[_Y_][_X_ + 1] = 1;
    game_map[_Y_+ 1][_X_ + 1] = 1;
    game_map[_Y_ + 1][_X_ + 2] = 1;
    game_map[_Y_ + 2][_X_ + 2] = 1;
    
    map_y = 0;
    tetraminoe_number = 0;
    return;
  }

  // increment axis y on game_map
  map_y++;
}

void drawBlock(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + pgm_read_byte(&tetraminoe_coordinates[i]) * SIZE;
    uint8_t start_y = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]) * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, WHITE);
  }
}
void cancelBlock(uint8_t x, uint8_t y, const uint8_t tetraminoe_coordinates[8]) {
  for (uint8_t i = 0; i < 8; i += 2) {
    uint8_t start_x = x + pgm_read_byte(&tetraminoe_coordinates[i]) * SIZE;
    uint8_t start_y = y + pgm_read_byte(&tetraminoe_coordinates[i + 1]) * SIZE;
    display.drawRect(start_x, start_y, SIZE, SIZE, BLACK);
  }
}