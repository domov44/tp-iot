#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "Babasque";
const char* password = "motdepasse";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

int Led = 26;
int bouton = 2;
int score = 0;
int Shock = 21;
int oldVal = LOW;
int val;
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 500;

#include "SevSeg.h"
SevSeg sevseg;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[%u] Déconnecté!\n", num);
        break;
      }
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connecté à: %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        break;
      }
    case WStype_TEXT:
      {
        String message = String((char*)payload);
        Serial.printf("[%u] Reçu un message texte: %s\n", num, message.c_str());
        break;
      }
    case WStype_BIN:
      {
        Serial.printf("[%u] Reçu un message binaire de longueur: %u\n", num, length);
        break;
      }
    default:
      break;
  }
}

void setup() {
  pinMode(Led, OUTPUT);
  pinMode(Shock, INPUT);
  pinMode(bouton, INPUT);
  Serial.begin(9600);

  byte numDigits = 4;
  byte digitPins[] = { 32, 13, 14, 15 };
  byte segmentPins[] = { 33, 25, 22, 19, 18, 12, 23, 5 };
  bool updateWithDelays = false;
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, updateWithDelays);
  sevseg.setBrightness(100);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", HTTP_GET, []() {
    String html = "<!DOCTYPE html>";
    html += "<html>";
    html += "<head>";
    html += "<title>S&eacute;rie d'abdominaux</title>";
    html += "<style>";
    html += "body {";
    html += "font-family: Arial, sans-serif;";
    html += "background-color: #f0f0f0;";
    html += "text-align: center;";
    html += "}";
    html += ".container {";
    html += "margin-top: 50px;";
    html += "padding: 20px;";
    html += "background-color: #fff;";
    html += "border-radius: 10px;";
    html += "box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);";
    html += "}";
    html += "#score {";
    html += "font-size: 40px;";
    html += "font-weight: bold;";
    html += "color: #27ae60;";
    html += "}";
    html += "</style>";
    html += "</head>";
    html += "<body>";
    html += "<div class='container'>";
    html += "<h1>S&eacute;rie d'abdominaux</h1>";
    html += "<p>Positionnez vous et suivez votre nombre d'abdominaux en temps r&eacute;el</p>";
    html += "<div id='score'>" + String(score) + "</div>";
    html += "</div>";
    html += "<script type='text/javascript'>var socket = new WebSocket('ws://' + window.location.hostname + ':81/');socket.onmessage = function(event) {document.getElementById('score').innerHTML = event.data;};</script>";
    html += "</body>";
    html += "</html>";
    server.send(200, "text/html", html);
  });
  server.begin();

  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  webSocket.loop();

  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis();
    int etat = digitalRead(bouton);

    if (etat == HIGH) {
      score = 0;
    }
    shock();
  }

  sevseg.setNumber(score);
  sevseg.refreshDisplay();
}

void shock() {
  val = digitalRead(Shock);
  if (val == HIGH) {
    if (oldVal != HIGH) {
      score = score + 1;
      oldVal = HIGH;
    }
    digitalWrite(Led, HIGH);

    Serial.print("Mes d'abdos réalisés: ");
    Serial.println(score);

    if (score > 0) {
      webSocket.broadcastTXT("Vous avez fait " + String(score) + " abdominaux");
    }
  } else {
    oldVal = LOW;
    digitalWrite(Led, LOW);
    Serial.println("Je suis au repos");
  }
}
