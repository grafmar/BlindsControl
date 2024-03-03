#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <Adafruit_SSD1306.h>

#include "KeypadReader.h"

#include <EEPROM.h>


WiFiManager wm;
WiFiClient client;

KeypadReader keypadReader;

// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 m_display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

bool atStartup = true;
bool configIP = false;
uint8_t IP[4];
uint8_t newIP[4];
uint8_t editPos;
bool posEdited;
uint32_t lastPressed = 0;
String cmd1 = "";
String cmd2 = "";
String cmd3 = "";
const char UP = static_cast<char>(0x18);
const char DN = static_cast<char>(0x19);

void setup() {
  EEPROM.begin(512);

  m_display.setRotation(0);
  if(!m_display.begin(SSD1306_NORMALDISPLAY, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  m_display.clearDisplay();
  m_display.display();

  wm.autoConnect("RemoteSteuerung_AP");  // anonymous ap

  // Wait for connection
  m_display.setTextSize(1);
  m_display.setTextColor(SSD1306_WHITE);
  m_display.setCursor(0,0);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    m_display.print(".");
    m_display.display();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  m_display.clearDisplay();
  m_display.setCursor(0,0);
  m_display.print("Connected to ");
  m_display.println(WiFi.SSID());
  m_display.println("IP address: ");
  m_display.println(WiFi.localIP());
  m_display.display();

  delay(2000);

  for(uint8_t i=0; i<4;i++) {
    IP[i] = EEPROM.read(i);
  }
}

void loop() {
  char key = keypadReader.getKey();

  if (atStartup) {
    atStartup = false;
    if (key != KeypadReader::NO_KEY) {
      handleConfigIP(KeypadReader::NO_KEY);
    }
  }
  else if (configIP) {
    handleConfigIP(key);
  }

  else if (key != KeypadReader::NO_KEY) {
    lastPressed = millis();
    String url = urlOfKey(key);

    m_display.clearDisplay();
    m_display.setTextColor(SSD1306_WHITE);
    m_display.setTextSize(2);
    m_display.setCursor(0,0);
    m_display.print(cmd1);
    m_display.setCursor(40,0);
    m_display.print(cmd2);
    m_display.setCursor(80,0);
    m_display.print(cmd3);
    m_display.setTextSize(1);
    m_display.setCursor(0,24);
    m_display.println(url.substring(7));
    m_display.display();

    HTTPClient http;
    http.begin(client, url);
    int httpCode = http.GET();

    m_display.setTextSize(1);
    m_display.setCursor(0,24);
    m_display.println(url.substring(7) + "->" + httpCode);
    m_display.display();

    http.end();
  }

  else if (millis() - lastPressed > 5000) {
    cmd1="";
    cmd2="";
    cmd3="";
    m_display.clearDisplay();
    m_display.display();
  }
}

String baseURL() {
  char ipString[16];
  sprintf(ipString, "%d.%d.%d.%d",IP[0],IP[1],IP[2],IP[3]);
  return String("http://") + ipString + "/";
}

String urlOfKey(char key) {
  String url = baseURL();

  switch(key) {
    case '1':
      url += "l0";
      cmd1 = String("1") + UP + UP;
      break;
    case '2':
      url += "l2";
      cmd2 = String("2") + UP + UP;
      break;
    case '3':
      url += "l4";
      cmd3 = String("3") + UP + UP;
      break;
    case '4':
      url += "s0";
      cmd1 = String("1") + UP;
      break;
    case '5':
      url += "s2";
      cmd2 = String("2") + UP;
      break;
    case '6':
      url += "s4";
      cmd3 = String("3") + UP;
      break;
    case '7':
      url += "s1";
      cmd1 = String("1") + DN;
      break;
    case '8':
      url += "s3";
      cmd2 = String("2") + DN;
      break;
    case '9':
      url += "s5";
      cmd3 = String("3") + DN;
      break;
    case '<':
      url += "l1";
      cmd1 = String("1") + DN + DN;
      break;
    case '0':
      url += "l3";
      cmd2 = String("2") + DN + DN;
      break;
    case '>':
      url += "l5";
      cmd3 = String("3") + DN + DN;
      break;
  }
  return url;
}


void handleConfigIP(char key) {
  bool updateScreen = false;
  if (!configIP) { // first enter
    updateScreen = true;
    configIP = true;
    for(uint8_t i=0 ; i<4 ; i++) {
      newIP[i]=IP[i];
    }
    editPos = 0;
    posEdited = false;
  }

  if (key != KeypadReader::NO_KEY) {
    updateScreen = true;

    if (key == '<') { // backspace / delete
      if (posEdited) {
        if (newIP[editPos] > 9) {
          newIP[editPos] = newIP[editPos] / 10;
        }
        else {
          newIP[editPos] = IP[editPos];
          posEdited = false;
        }
      }
      else if (editPos == 0) {
        // do nothing
      }
      else {
        editPos--;
        newIP[editPos] = IP[editPos];
        posEdited = false;
      }
    }

    else if (key == '>') {
      if (editPos < 3) {
        editPos++;
        posEdited=false;
      }
      else { // finished
        for(uint8_t i=0 ; i<4 ; i++) {
          IP[i] = newIP[i];
        }
        configIP = false;
          for(uint8_t i=0; i<4;i++) {
            EEPROM.write(i,IP[i]);
          }
          EEPROM.commit();
      }
    }

    else {
      if (!posEdited) {
        newIP[editPos] = 0;
        posEdited = true;
      }
      if (newIP[editPos] <= 99) {
        newIP[editPos] = newIP[editPos] * 10 + static_cast<uint8_t>(key - '0');
      }
      if ((newIP[editPos] > 99) && (editPos < 3)) {
        editPos++;
        newIP[editPos] = IP[editPos];
        posEdited = false;
      }
    }
  }

  if (updateScreen) {
    m_display.clearDisplay();
    if (configIP) {
      const uint8_t xx = editPos * 4*6;
      const uint8_t yy = 16;
      const uint8_t ww = 3*6+3;
      const uint8_t hh = 9;
      m_display.setTextColor(SSD1306_WHITE);
      m_display.setTextSize(1);
      m_display.setCursor(0,0);
      m_display.println("Config IP:");
      m_display.setCursor(2,yy+1);
      char ipString[16];
      sprintf(ipString, "%3d.%3d.%3d.%3d",newIP[0],newIP[1],newIP[2],newIP[3]);
      m_display.println(ipString);
      if (posEdited) {
        m_display.fillRect(xx,yy,ww,hh,SSD1306_INVERSE);
      }
      else {
        m_display.drawRect(xx,yy-1,ww,hh+2,SSD1306_WHITE);
      }
      m_display.display();
    }
    m_display.display();
  }
}
