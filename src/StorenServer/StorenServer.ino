#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

WiFiManager wm;
  
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int led = LED_BUILTIN;
const int relays[] = {D0,D1,D2,D3,D6,D5};
static const uint8_t NUM_OF_RELAYS = 6;

const char HTML[] PROGMEM = R"====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Storensteuerung</title>
    <style>
        body{margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}
        table{border-collapse:collapse}
        th,td{padding:5px;padding-right:50px;font-size:50px;text-align:center}
        a{text-decoration:none;color:inherit}
        button{width:100px;height:100px;font-size:50px}
    </style>
</head>
<body>
    <table>
        <tr><th>1</th><th>2</th><th>3</th></tr>
        <tr><td><a href="/l0"><button>&#8607;</button></a></td><td><a href="/l2"><button>&#8607;</button></a></td><td><a href="/l4"><button>&#8607;</button></a></td></tr>
        <tr><td><a href="/s0"><button>&#8593;</button></a></td><td><a href="/s2"><button>&#8593;</button></a></td><td><a href="/s4"><button>&#8593;</button></a></td></tr>
        <tr><td><a href="/s1"><button>&#8595;</button></a></td><td><a href="/s3"><button>&#8595;</button></a></td><td><a href="/s5"><button>&#8595;</button></a></td></tr>
        <tr><td><a href="/l1"><button>&#8609;</button></a></td><td><a href="/l3"><button>&#8609;</button></a></td><td><a href="/l5"><button>&#8609;</button></a></td></tr>
    </table>
</body>
</html>
)====";

static const uint32_t SHORT_PULSE = 100;
static const uint32_t LONG_PULSE = 500;
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

void setup(void) {
  for(uint8_t i=0U; i<NUM_OF_RELAYS;i++) {
    digitalWrite(relays[i], 0);
    pinMode(relays[i], OUTPUT);
    pulsing[i] = false;
  }
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  Serial.begin(115200);
  
  wm.autoConnect("StorenSteuerung_AP"); // anonymous ap

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
  server.on("/s0", []() { pulseRelay(0, SHORT_PULSE); handleRoot(); });
  server.on("/s1", []() { pulseRelay(1, SHORT_PULSE); handleRoot(); });
  server.on("/s2", []() { pulseRelay(2, SHORT_PULSE); handleRoot(); });
  server.on("/s3", []() { pulseRelay(3, SHORT_PULSE); handleRoot(); });
  server.on("/s4", []() { pulseRelay(4, SHORT_PULSE); handleRoot(); });
  server.on("/s5", []() { pulseRelay(5, SHORT_PULSE); handleRoot(); });
  server.on("/l0", []() { pulseRelay(0, LONG_PULSE);  handleRoot(); });
  server.on("/l1", []() { pulseRelay(1, LONG_PULSE);  handleRoot(); });
  server.on("/l2", []() { pulseRelay(2, LONG_PULSE);  handleRoot(); });
  server.on("/l3", []() { pulseRelay(3, LONG_PULSE);  handleRoot(); });
  server.on("/l4", []() { pulseRelay(4, LONG_PULSE);  handleRoot(); });
  server.on("/l5", []() { pulseRelay(5, LONG_PULSE);  handleRoot(); });
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

void handlePulses() {
    for(uint8_t i=0U; i<NUM_OF_RELAYS;i++) {
      if ((pulsing[i]) && (millis() > stoptime[i])) {
        pulsing[i] = false;
        digitalWrite(relays[i], 0);
        digitalWrite(led, 0);
      }
    }
}
