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
uint8_t x_axis = 0;
uint8_t y_axis = 0;
uint8_t tetraminoe_number;

bool game_map[20][10] = {
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
const bool SQUARE[2][2] PROGMEM = {
  {1, 1},
  {1, 1}
};
const bool ES[3][3] PROGMEM = {
  {0, 1, 0},
  {0, 1, 1},
  {0, 0, 1}
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
      drawSquare(x_axis, y_axis);
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
      drawEs(x_axis, y_axis);
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
  y_axis = y_axis < 18 ? y_axis + 1 : 0;
}
uint8_t generateRandomNumber() {
  return random(0, 7);
}
void drawSquare(uint8_t _X_, uint8_t _Y_) { 
  
  // cancel last square
  if (_Y_ > 0) {
    uint8_t old_Y_ = _Y_ - 1;    
    // conversion to real coordinates in pixels
    uint8_t x = 2 + _X_ * SIZE;
    uint8_t y = 4 + old_Y_ * SIZE;
    display.drawRect(x, y, SIZE, SIZE, BLACK);
    display.drawRect(x + SIZE, y, SIZE, SIZE, BLACK);
    display.drawRect(x, y + SIZE, SIZE, SIZE, BLACK);
    display.drawRect(x + SIZE, y + SIZE, SIZE, SIZE, BLACK);  
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
    game_map[_Y_][_X_] = 1;
    game_map[_Y_][_X_ + 1] = 1;
    game_map[_Y_ + 1][_X_ + 1] = 1;
    game_map[_Y_ + 1][_X_ + 1] = 1;
    
    y_axis = 0;
    tetraminoe_number = 5;
    return;
  }

  // increment axis y on game_map
  y_axis++;  
}
void drawEs(uint8_t _X_, uint8_t _Y_) {
  // cancel last es
  if (_Y_ > 0) {
    uint8_t old_Y_ = _Y_ - 1;    
    // conversion to real coordinates in pixels
    uint8_t x = 2 + _X_ * SIZE;
    uint8_t y = 4 + old_Y_ * SIZE;

    display.drawRect(x + SIZE, y, SIZE, SIZE, BLACK);
    display.drawRect(x + SIZE, y + SIZE, SIZE, SIZE, BLACK);
    display.drawRect(x + SIZE * 2, y + SIZE, SIZE, SIZE, BLACK);
    display.drawRect(x + SIZE * 2, y + SIZE * 2, SIZE, SIZE, BLACK); 
  }

  // conversion to real coordinates in pixels
  uint8_t x = 2 + _X_ * SIZE;
  uint8_t y = 4 + _Y_ * SIZE;

  // draw new square
  drawBlock2(x, y, ES);

  // update time
  last_time = millis(); 
  
  // handle stop tetraminoe
  if (_Y_ > 16 || game_map[_Y_ + 2][_X_]) {
    Serial.println("fine corsa");
    game_map[_Y_][_X_ + 1] = 1;
    game_map[_Y_+ 1][_X_ + 1] = 1;
    game_map[_Y_ + 1][_X_ + 2] = 1;
    game_map[_Y_ + 2][_X_ + 2] = 1;
    
    y_axis = 0;
    tetraminoe_number = 0;
    return;
  }

  // increment axis y on game_map
  y_axis++;
}

void drawBlock(uint8_t x, uint8_t y, const bool array[2][2]) {
  for (uint8_t i = 0; i < 2; i++) {
    // i = y
    uint8_t addsizey = i * SIZE;

    for (uint8_t j = 0; j < 2; j++) {
      // j = x
      uint8_t addsizex = j * SIZE;

      if (pgm_read_byte(&(SQUARE[i][j]))) {
        uint8_t start_x = x + addsizex;
        uint8_t start_y = y + addsizey;
        display.drawRect(start_x, start_y, SIZE, SIZE, WHITE);
      }
    }
  }
}
void drawBlock2(uint8_t x, uint8_t y, const bool array[3][3]) {
  for (uint8_t i = 0; i < 3; i++) {
    // i = y
    uint8_t addsizey = i * SIZE;

    for (uint8_t j = 0; j < 3; j++) {
      // j = x
      uint8_t addsizex = j * SIZE;

      if (pgm_read_byte(&(ES[i][j]))) {
        uint8_t start_x = x + addsizex;
        uint8_t start_y = y + addsizey;

        // Serial.print("X :");
        // Serial.println(start_x);
        // Serial.print("Y :");
        // Serial.println(start_y);

        display.drawRect(start_x, start_y, SIZE, SIZE, WHITE);
      }
    }
  }
}