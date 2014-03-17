/*
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int ledRed = 12;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10);             // wait in ms
  digitalWrite(ledRed, LOW);    // turn the LED off by making the voltage LOW
  delay(40);               // wait in ms
  digitalWrite(ledRed, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(70);             // wait in ms
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(10);               // wait in ms
}
