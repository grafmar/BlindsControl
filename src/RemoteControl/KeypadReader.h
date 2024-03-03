#ifndef KEYPADREADER_H
#define KEYPADREADER_H

#include <Arduino.h>

class KeypadReader {
public:
  static const char NO_KEY = '\0';
  
  explicit KeypadReader();
  ~KeypadReader();

  char getKey(void);

private:
  uint32_t m_resetMillis;
  bool m_resetStarted;
};

#endif // KEYPADREADER_H