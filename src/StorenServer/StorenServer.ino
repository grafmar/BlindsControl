#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

WiFiManager wm;
  
ESP8266WebServer server(80);

const int led = LED_BUILTIN;
//const int relays[] = {D0,D1,D2,D3,D6,D5};
//const int relays[] = {0,1,D2,D3,D6,D5};
const int relays[] = {16,5,D2,D3,D6,D5};


const char HTML[] PROGMEM = R"====(
<!DOCTYPE HTML>
<html>

<head>
    <title>Storensteuerung</title>
</head>

<body>
    <a href="/s0"><button>s0</button></a>
    <a href="/s1"><button>s1</button></a>
    <a href="/s2"><button>s2</button></a>
    <a href="/s3"><button>s3</button></a>
    <a href="/s4"><button>s4</button></a>
    <a href="/s5"><button>s5</button></a>
    <br>
    <a href="/l0"><button>l0</button></a>
    <a href="/l1"><button>l1</button></a>
    <a href="/l2"><button>l2</button></a>
    <a href="/l3"><button>l3</button></a>
    <a href="/l4"><button>l4</button></a>
    <a href="/l5"><button>l5</button></a>
    <br>
</body>
</html>
)====";

uint32_t SHORT_PULSE = 100;
uint32_t LONG_PULSE = 500;

void pulseRelay(uint8_t relayNum, uint32_t time) {
    digitalWrite(relays[relayNum], 1);
    digitalWrite(led, 1);
    delay(time);
    digitalWrite(relays[relayNum], 0);
    digitalWrite(led, 0);
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
  for(uint8_t i=0U; i<6;i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], 0);
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
}
