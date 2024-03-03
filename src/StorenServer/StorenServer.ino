#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#include "raw_html.h"

WiFiManager wm;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int led = LED_BUILTIN;
const int relays[] = { D0, D1, D2, D3, D6, D5 };
static const uint8_t NUM_OF_RELAYS = 6;

static const uint32_t SHORT_PULSE = 100;
static const uint32_t LONG_PULSE = 1000;
bool pulsing[NUM_OF_RELAYS];
uint32_t stoptime[NUM_OF_RELAYS];


void pulseRelay(uint8_t relayNum, uint32_t time) {
  digitalWrite(relays[relayNum], 1);
  digitalWrite(led, 1);
  pulsing[relayNum] = true;
  stoptime[relayNum] = millis() + time;
}

void handleRoot() {
  server.send(200, "text/html", HTML);
}

void redirectToRoot() {
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
}

void handlePulses() {
  for (uint8_t i = 0U; i < NUM_OF_RELAYS; i++) {
    if ((pulsing[i]) && (millis() > stoptime[i])) {
      pulsing[i] = false;
      digitalWrite(relays[i], 0);
      digitalWrite(led, 0);
    }
  }
}

void setup(void) {
  for (uint8_t i = 0U; i < NUM_OF_RELAYS; i++) {
    digitalWrite(relays[i], 0);
    pinMode(relays[i], OUTPUT);
    pulsing[i] = false;
  }
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  Serial.begin(115200);

  wm.autoConnect("StorenSteuerung_AP");  // anonymous ap

  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("Storensteuerung")) { Serial.println("MDNS responder started"); }
  MDNS.addService("http", "tcp", 80);

  httpUpdater.setup(&server);

  /////////////////////////////////////////////////////////
  // Handlers
  server.on("/", handleRoot);
  server.on("/s0", []() {
    pulseRelay(0, SHORT_PULSE);
    redirectToRoot();
  });
  server.on("/s1", []() {
    pulseRelay(1, SHORT_PULSE);
    redirectToRoot();
  });
  server.on("/s2", []() {
    pulseRelay(2, SHORT_PULSE);
    redirectToRoot();
  });
  server.on("/s3", []() {
    pulseRelay(3, SHORT_PULSE);
    redirectToRoot();
  });
  server.on("/s4", []() {
    pulseRelay(4, SHORT_PULSE);
    redirectToRoot();
  });
  server.on("/s5", []() {
    pulseRelay(5, SHORT_PULSE);
    redirectToRoot();
  });
  server.on("/l0", []() {
    pulseRelay(0, LONG_PULSE);
    redirectToRoot();
  });
  server.on("/l1", []() {
    pulseRelay(1, LONG_PULSE);
    redirectToRoot();
  });
  server.on("/l2", []() {
    pulseRelay(2, LONG_PULSE);
    redirectToRoot();
  });
  server.on("/l3", []() {
    pulseRelay(3, LONG_PULSE);
    redirectToRoot();
  });
  server.on("/l4", []() {
    pulseRelay(4, LONG_PULSE);
    redirectToRoot();
  });
  server.on("/l5", []() {
    pulseRelay(5, LONG_PULSE);
    redirectToRoot();
  });
  /////////////////////////////////////////////////////////

  server.begin();
  Serial.println("HTTP server started");

  ArduinoOTA.begin();
}

void loop(void) {
  server.handleClient();
  ArduinoOTA.handle();
  MDNS.update();
  handlePulses();
}
