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

//Define connections
const int trigger0Pin = A0;
const int trigger1Pin = A1;
const int trigger2Pin = A2;
const int trigger3Pin = A3;
const int trigger4Pin = A4;

const int ledPin = 13;

const int motorPhaseAPin = 2;
const int motorPhaseBPin = 3;
const int motorPhaseAInterrupt = 0;

//Define environmental variables
const int triggerPulseLength_us = 1000; //in us
const int triggerShutter = trigger0Pin;
const int triggerCamera0 = trigger1Pin;
const int triggerCamera1 = trigger2Pin;
const int triggerCamera2 = trigger3Pin;
const int triggerCamera3 = trigger4Pin;

//Define Global Variables
volatile int targetPosition = 0;
volatile int targetShotUntil = 0;
volatile int targetShotsFired = 0;
volatile int targetRotations = 0;
boolean targetPositionChanged = 0;



const int pulsesPerRound = 1800; //1800 was the original value
const float degPerPulse = 1.0 / pulsesPerRound * 360.0;


int sequenceLength = 500; //duration of registrateable shots in ms
int shutterOpeningTime = 20;
int shutterClosingTime = 20;


//error handling routinge. Keeps the LED as long on as the error has been cleared
void error(String str) {
  digitalWrite(ledPin, 1);
  Serial.println("ERROR: " + str);
}

//shooting execution routine  //should probably go in the main loop under the work that is executed each x ms
boolean shootingRoutine(void) {

  //check if we have space!!!
  if (targetShotsFired) {
    int freeSpace = pulsesPerRound - targetShotUntil;
    int averageSpaceUsed = targetShotUntil / targetShotsFired;
    averageSpaceUsed += averageSpaceUsed / 10; //give it plus 10% for security
    if ((freeSpace - averageSpaceUsed) <= 0 ) return 0;
  }


  while ( targetPosition != (targetShotUntil) ); //pay attention to declare the variable as volatile otherwise the ide optimizes the check out
   

  digitalWrite(triggerShutter, 0);
  delay(shutterOpeningTime);
  digitalWrite(triggerCamera0, 0);
  digitalWrite(triggerCamera1, 0);
  digitalWrite(triggerCamera2, 0);
  digitalWrite(triggerCamera3, 0);
  delayMicroseconds(triggerPulseLength_us);
  digitalWrite(triggerCamera0, 1);
  digitalWrite(triggerCamera1, 1);
  digitalWrite(triggerCamera2, 1);
  digitalWrite(triggerCamera3, 1);
  delay(sequenceLength - (triggerPulseLength_us / 1000));
  digitalWrite(triggerShutter, 1);
  delay(shutterClosingTime);

  if (targetPosition < targetShotUntil) error("Target Overshot");

  targetShotUntil = targetPosition + 1;
  targetShotsFired++;
  return 1;
}

float absToDeg(int pos){
  return degPerPulse * pos;
}


//command execution routine
void commandExecute(String command) {
  if (command == "shoot") {
    Serial.println("ok");
    if (shootingRoutine()) {
      Serial.println(absToDeg(targetShotUntil));
    } else {
      error("target full");
    }
  } else if (command == "resetTarget") {
    targetShotUntil = 0;
    Serial.println("Ok");
  } else if (command == "triggerCameras") {
    Serial.println("Ok");
  } else if (command == "targetPosition") {
    Serial.println(absToDeg(targetPosition));
  } else if (command == "calibrate") {
    Serial.println("Ok");
    targetPosition = 0;
  } else if (command == "delay") {
    Serial.println("Ok");
    delay(500);
  }

}




// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  digitalWrite(trigger0Pin, 1);
  digitalWrite(trigger1Pin, 1);
  digitalWrite(trigger2Pin, 1);
  digitalWrite(trigger3Pin, 1);
  digitalWrite(trigger4Pin, 1);


  pinMode(trigger0Pin, OUTPUT);
  pinMode(trigger1Pin, OUTPUT);
  pinMode(trigger2Pin, OUTPUT);
  pinMode(trigger3Pin, OUTPUT);
  pinMode(trigger4Pin, OUTPUT);

  pinMode(motorPhaseAPin, INPUT_PULLUP);
  pinMode(motorPhaseBPin, INPUT_PULLUP);

  attachInterrupt(motorPhaseAInterrupt, interruptRoutine, FALLING);

  Serial.begin(9600);
}

// the loop routine runs over and over again forever:

long timeLastWork = 0;
const int timeWorkPeriod = 1000;
const int commandBufferLength = 20;
char commandBuffer[commandBufferLength];
int commandBufferPosition = 0;
String command;

void loop()
{


  //do something every x miliseconds
  if ((millis() - timeLastWork) >= timeWorkPeriod) {
    timeLastWork += timeWorkPeriod;
  }


  //receive incoming comunication and check if we have to execute a command
  if (Serial.available() != 0) {

    if (commandBufferPosition == commandBufferLength) {
      error("commandBuffer overflow");
      Serial.flush();
      commandBufferPosition = 0;
    } else {
      commandBuffer[commandBufferPosition] = Serial.read();
    }

    switch (commandBuffer[commandBufferPosition]) {
      case '!':
        commandBuffer[commandBufferPosition] = 0;
        command = String(commandBuffer);
        commandExecute(command);
        commandBufferPosition = 0;
        break;
      case '\n':
      case '\r':
        commandBufferPosition = 0;
        break;
      default:
        commandBufferPosition++;
        break;
    }
  }


  if (targetPositionChanged) {
    targetPositionChanged = 0;
    //Serial.println(targetPositionDeg);
  }


}

void interruptRoutine() {

  digitalWrite(ledPin, !digitalRead(ledPin));

  if (digitalRead(motorPhaseBPin)) {
    targetPosition++;
  } else {
    targetPosition--;
  }

  if (targetPosition >= pulsesPerRound) {
    targetPosition = targetPosition - pulsesPerRound;
    targetRotations++;
  } else if (targetPosition < 0) {
    targetPosition = targetPosition + pulsesPerRound;
    targetRotations--;
  }

  targetPositionChanged = 1;
}


