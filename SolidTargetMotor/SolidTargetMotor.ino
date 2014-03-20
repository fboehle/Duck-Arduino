/**********************************************************
*	Test Program for Arduino
*	
*	Developement started: 03/2014
*	Author: Frederik Böhle code@fboehle.de
*
**********************************************************
*   
*   Description: This code counts the number of interrupts on pin 2
*   and sends this value over the serial port
*
*   Notes:
*
*   Changelog: 
*
**********************************************************/


// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int ledOnboardPin = 13;
int interruptNumber = 0;
int interruptPin = 2;
int phasePin = 3;

float targetPosition = 0.0;
int targetRotations = 0;
boolean changeHappened = 0;


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(ledOnboardPin, OUTPUT);     
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(phasePin, INPUT_PULLUP);
  attachInterrupt(interruptNumber, interruptRoutine, FALLING);
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
  const int sentEvery = 10;
  int lastSent = sentEvery;
void loop() 
{
  if(changeHappened){
    changeHappened = 0;
    
    if( lastSent == sentEvery ){
      Serial.print(targetPosition);
      Serial.print("\n");
      lastSent = 0;
    }
    lastSent ++;
    digitalWrite(ledOnboardPin, LOW);
  }
  

}

void interruptRoutine()
{
  digitalWrite(ledOnboardPin, HIGH);
  const float pulsesPerRound = 1800;
  const float degPerPulse = 0.2; // 1/pulsesPerRound*360
  
  if(digitalRead(phasePin)){
    targetPosition = targetPosition - degPerPulse;
  }else{
    targetPosition = targetPosition + degPerPulse;
  }
  
  if(targetPosition >= 359.99){
    targetPosition = 0;
    targetRotations++;
  }else if(targetPosition < 0){
    targetPosition = targetPosition + 360;
    targetRotations--;
  }
  
  

  changeHappened = 1;
}


