#include "KeypadReader.h"
#include <Keypad.h>

// Keypad
static const byte ROWS = 4; //four rows
static const byte COLS = 3; //three columns
static const char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'<','0','>'}
};
static byte rowPins[ROWS] = {14, 1, 3, 13}; //connect to the row pinouts of the keypad
static byte colPins[COLS] = {12,16, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );

KeypadReader::KeypadReader() :
  m_resetMillis(0),
  m_resetStarted(false)
{
}

KeypadReader::~KeypadReader() {
}

char KeypadReader::getKey(void) {
  char key = keypad.getKey();

  return key;
}
