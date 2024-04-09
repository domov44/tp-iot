int Led = 26;    // LED on Arduino board
int Shock = 21;  // sensor signal
int val;         // numeric variable to store sensor status
unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 100; // Interval to update the shock sensor, in milliseconds
 
#include "SevSeg.h"
SevSeg sevseg;
 
void setup() {
  pinMode(Led, OUTPUT);   // define LED as output interface
  pinMode(Shock, INPUT);  // define input for sensor signal
  Serial.begin(9600);
 
  byte numDigits = 4;
  byte digitPins[] = { 32, 13, 14, 15 };
  byte segmentPins[] = { 33, 25, 22, 19, 18, 12, 23, 5 };
  bool updateWithDelays = false;
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, updateWithDelays);
  sevseg.setBrightness(100);
}
 
void loop() {
  // Check if it's time to update the shock sensor
  if (millis() - lastUpdateTime >= updateInterval) {
    lastUpdateTime = millis(); // Update the last update time
 
    shock(); // appel de la fonction shock pour vérifier le capteur de vibration
  }
 
  sevseg.setNumber(1234); // Mise à jour de l'affichage directement ici
  sevseg.refreshDisplay(); // Mettre à jour l'affichage
}
 
void shock() {
  val = digitalRead(Shock);  // read and assign the value of digital interface 3 to val
  if (val == HIGH)           // when sensor detects a signal, the LED flashes
  {
    digitalWrite(Led, HIGH);
    Serial.println("Im high");
  } else {
    digitalWrite(Led, LOW);
    Serial.println("Im low");
  }
}