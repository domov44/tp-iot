int Led = 25;    // LED on Arduino board
int Shock = 32;  // sensor signal
int buzzer = 23;
int val;  // numeric variable to store sensor status

void setup() {
  pinMode(Led, OUTPUT);     // define LED as output interface
  pinMode(Shock, INPUT);    // define input for sensor signal
  pinMode(buzzer, OUTPUT);  // set pin 8 as output
  Serial.begin(9600);
}

void loop() {


  val = digitalRead(Shock);  // read and assign the value of digital interface 3 to val
  if (val == HIGH)           // when sensor detects a signal, the LED flashes
  {
    digitalWrite(Led, HIGH);
    unsigned char i;
    while (1) {
      // In this program, the buzzer is controlled alternately with two different frequencies.
      // The signal consists of a square wave voltage.
      // Turning the buzzer on and off will generate a tone that roughly corresponds to the frequency.
      // The frequency is defined by the length of the on and off phase.

      //Frequency 1
      for (i = 0; i < 80; i++) {
        digitalWrite(buzzer, HIGH);
        delay(1);
        digitalWrite(buzzer, LOW);
        delay(1);
      }
      //Frequency 2
      for (i = 0; i < 100; i++) {
        digitalWrite(buzzer, HIGH);
        delay(2);
        digitalWrite(buzzer, LOW);
        delay(2);
      }
    }
    Serial.println("Im high");
    delay(500);
  } else {
    digitalWrite(Led, LOW);
    Serial.println("Im low");
    delay(500);
  }
  delay(1000);
}