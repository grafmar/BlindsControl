#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

WiFiManager wm;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

const int led = LED_BUILTIN;
const int relays[] = { D0, D1, D2, D3, D6, D5 };
static const uint8_t NUM_OF_RELAYS = 6;

const char HTML[] PROGMEM = R"====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Storensteuerung</title>
	<link rel="shortcut icon" href="data:image/x-icon;base64,AAABAAEAEBACAAEAAQCwAAAAFgAAACgAAAAQAAAAIAAAAAEAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////AAAAAAB//gAAfv4AAH7+AAB+/gAAfv4AAH7+AAB6vgAAfH4AAH7+AAD//wAAAAAAAP//AAAAAAAA//8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" />
    <style>
        body{margin:0;padding:0;display:flex;justify-content:center;align-items:center;height:100vh}
        table{border-collapse:collapse}
        th,td{padding:5px;padding-left:20px;padding-right:20px;font-size:50px;text-align:center}
        a{text-decoration:none;color:inherit}
        button{width:100px;height:100px;font-size:50px}
    </style>
</head>
<body>
    <table>
        <tr><th>1</th><th>2</th><th>3</th></tr>
        <tr><td><button title="NumpadSubtract" onclick="sendCtrl('/l0')">&#8607;</button></td><td><button title="NumpadDivide" onclick="sendCtrl('/l2')">&#8607;</button></td><td><button title="NumpadMultiply" onclick="sendCtrl('/l4')">&#8607;</button></td></tr>
        <tr><td><button title="Numpad7" onclick="sendCtrl('/s0')">&#8593;</button></td><td><button title="Numpad8" onclick="sendCtrl('/s2')">&#8593;</button></td><td><button title="Numpad9" onclick="sendCtrl('/s4')">&#8593;</button></td></tr>
        <tr><td><button title="Numpad4" onclick="sendCtrl('/s1')">&#8595;</button></td><td><button title="Numpad5" onclick="sendCtrl('/s3')">&#8595;</button></td><td><button title="Numpad6" onclick="sendCtrl('/s5')">&#8595;</button></td></tr>
        <tr><td><button title="Numpad1" onclick="sendCtrl('/l1')">&#8609;</button></td><td><button title="Numpad2" onclick="sendCtrl('/l3')">&#8609;</button></td><td><button title="Numpad3" onclick="sendCtrl('/l5')">&#8609;</button></td></tr>
    </table>
<script>
function sendCtrl(url) {
  const xhr = new XMLHttpRequest();
  xhr.open("GET", url);
  xhr.send();
}
document.onkeydown = function(e) {
  if (e.code == "NumpadSubtract") {e.preventDefault();sendCtrl('/l0'); }
  if (e.code == "Numpad7")        {e.preventDefault();sendCtrl('/s0'); }
  if (e.code == "Numpad4")        {e.preventDefault();sendCtrl('/s1'); }
  if (e.code == "Numpad1")        {e.preventDefault();sendCtrl('/l1'); }
  
  if (e.code == "NumpadDivide")   {e.preventDefault();sendCtrl('/l2'); }
  if (e.code == "Numpad8")        {e.preventDefault();sendCtrl('/s2'); }
  if (e.code == "Numpad5")        {e.preventDefault();sendCtrl('/s3'); }
  if (e.code == "Numpad2")        {e.preventDefault();sendCtrl('/l3'); }
  
  if (e.code == "NumpadMultiply") {e.preventDefault();sendCtrl('/l4'); }
  if (e.code == "Numpad9")        {e.preventDefault();sendCtrl('/s4'); }
  if (e.code == "Numpad6")        {e.preventDefault();sendCtrl('/s5'); }
  if (e.code == "Numpad3")        {e.preventDefault();sendCtrl('/l5'); }
};
</script>
</body></html>
)====";

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

void handlePulses() {
  for (uint8_t i = 0U; i < NUM_OF_RELAYS; i++) {
    if ((pulsing[i]) && (millis() > stoptime[i])) {
      pulsing[i] = false;
      digitalWrite(relays[i], 0);
      digitalWrite(led, 0);
    }
  }
}
