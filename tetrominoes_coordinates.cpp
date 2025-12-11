#include <arduino.h>
#include "tetrominoes_coordinates.h"

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