#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "SevSeg.h"

const char* ssid = "Babasque";
const char* password = "motdepasse";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const int Led = 26;
const int bouton = 2;
const int Shock = 21;
int score = 0;
int oldVal = LOW;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 500;

SevSeg sevseg;

// Websocket 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Déconnecté!\n", num);
      break;
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
      Serial.printf("[%u] Reçu un message binaire de longueur: %u\n", num, length);
      break;
    default:
      break;
  }
}

// Se connecter à la wifi
void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Setup de l'écran
void setupSevSeg() {
  byte numDigits = 4;
  byte digitPins[] = { 32, 13, 14, 15 };
  byte segmentPins[] = { 33, 25, 22, 19, 18, 12, 23, 5 };
  bool updateWithDelays = false;
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, updateWithDelays);
  sevseg.setBrightness(100);
}

// Setup du serveur et du rendu HTML
void setupServer() {
  server.on("/", HTTP_GET, []() {
    String html = "<!DOCTYPE html><html><head><title>S&eacute;rie d'abdominaux</title><style>body {font-family: Arial, sans-serif;background-color: #f0f0f0;text-align: center;}.container {margin-top: 50px;padding: 20px;background-color: #fff;border-radius: 10px;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);}#score {font-size: 40px;font-weight: bold;color: #27ae60;}</style></head><body><div class='container'><h1>S&eacute;rie d'abdominaux</h1><p>Positionnez vous et suivez votre nombre d'abdominaux en temps r&eacute;el</p><div id='score'>Vous avez fait " + String(score) + " abdominaux</div></div><script type='text/javascript'>var socket = new WebSocket('ws://' + window.location.hostname + ':81/');socket.onmessage = function(event) {document.getElementById('score').innerHTML = event.data;};</script></body></html>";
    server.send(200, "text/html", html);
  });
  server.begin();
}

void setup() {
  pinMode(Led, OUTPUT);
  pinMode(Shock, INPUT);
  pinMode(bouton, INPUT);
  Serial.begin(9600);

  setupWiFi();
  setupSevSeg();
  setupServer();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

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
      webSocket.broadcastTXT("Vous avez fait " + String(score) + " abdominaux");
    }
    if (digitalRead(Shock) == HIGH) {
      if (oldVal != HIGH) {
        score++;
        oldVal = HIGH;
        digitalWrite(Led, HIGH);
        Serial.print("Mes d'abdos réalisés: ");
        Serial.println(score);
        if (score > 0) {
          webSocket.broadcastTXT("Vous avez fait " + String(score) + " abdominaux");
        }
      }
    } else {
      oldVal = LOW;
      digitalWrite(Led, LOW);
      Serial.println("Je suis au repos");
    }
  }

  sevseg.setNumber(score);
  sevseg.refreshDisplay();
}