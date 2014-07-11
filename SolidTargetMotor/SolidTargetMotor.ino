/**********************************************************
 *	Arduino Motor Controll
 *
 *	Developement started: 03/2014
 *	Author: Frederik Böhle code@fboehle.de
 *
 **********************************************************
 *
 *   Description: Program that uses the encoder pulses from the target motor to accurately control the timing of the laser shutter. The encoder pulses generate an interrupt which keeps track of the target position. The mainloops waits until a full command over the serial port has been send and then deligates it to a subroutine.  
 *
 *   Notes: TODO: Reintegrate the flip blocker
 *
 *   Changelog:
 *
 ********************************************************** 
 *
Communication protocoll:

COMMANDS:
Receives: [command]!
Sends: 'ok' or error message

Available Commands:
-calibrate
-triggerCameras
-resetTarget
-shoot=[timeInMs]


QUERRIES:
Receives: [querry]?
Sends: [value]

Available Querries:
-targetPosition
-shotUntil
 *
 **********************************************************/
 
 

//#define DEBUG


//Define hardware connections to the uC
const int trigger0 = A0;
const int trigger1 = A1;
const int trigger2 = A2;
const int trigger3 = A3;
const int trigger4 = A4;

const int ledPin = 13;


const int optocoupledInput0 = 2;
const int optocoupledInput1 = 3;

const int openCollectorOutput0 = 22;
const int openCollectorOutput1 = 24;
const int openCollectorOutput2 = 26;
const int openCollectorOutput3 = 28;
const int openCollectorOutput4 = 30;
const int openCollectorOutput5 = 32;
const int openCollectorOutput6 = 34;
const int openCollectorOutput7 = 36;
const int openCollectorOutput8 = 38;
const int openCollectorOutput9 = 40;
const int openCollectorOutput10 = 42;
const int openCollectorOutput11 = 44;


//Define environmental variables
const int triggerPulseLength_us = 1000; //in us

const int melservoSON = openCollectorOutput0;
const int melservoST1 = openCollectorOutput1;
const int melservoST2 = openCollectorOutput2;
const int melservoSP1 = openCollectorOutput3;
const int melservoSP2 = openCollectorOutput4;

const int motorPhaseAPin = optocoupledInput0;
const int motorPhaseBPin = optocoupledInput1;
const int motorPhaseAInterrupt = 0;

const int triggerShutter = trigger0;

const int triggerCamera0 = trigger1;
const int triggerCamera1 = trigger2;
const int triggerCamera2 = trigger3;

const int triggerFlipBlocker = trigger4;

const int pulsesPerRound = 1800; //1800 was the original value
const float degPerPulse = 1.0 / pulsesPerRound * 360.0;

const int shutterOpeningTime = 15;
const int shutterClosingTime = 15;

//Define Global Variables
volatile int targetPosition = 0;
volatile int targetShotUntil = 0;
volatile int targetShotsFired = 0;
volatile int targetRotations = 0;
volatile boolean targetPositionChanged = 0;

//error handling routine. Keeps the LED as long on as the error has been cleared
void error(String str) {
  digitalWrite(ledPin, 1);
  Serial.println("ERROR: " + str);
}

//return an OK message
void ok(void){
  Serial.println("ok");
}



void melservoStart(void){
  digitalWrite(melservoSON, 1);

}
void melservoStop(void){
  digitalWrite(melservoSON, 0);

}
void melservoSpeed(void){
}
void melservoInitialize(void){
  digitalWrite(melservoST1, 1);
  digitalWrite(melservoST2, 0);
  digitalWrite(melservoSP1, 0);
  digitalWrite(melservoSP2, 0);
    
  
}
void triggerCameras(void){
  digitalWrite(triggerCamera0, 0);
  digitalWrite(triggerCamera1, 0);
  digitalWrite(triggerCamera2, 0);
  delayMicroseconds(triggerPulseLength_us);
  digitalWrite(triggerCamera0, 1);
  digitalWrite(triggerCamera1, 1);
  digitalWrite(triggerCamera2, 1);
}

//shooting execution routine  
void shootingRoutine(int sequenceLength) {

  //check if we have space! This is still something basic and not completely reliable. Will be rewritten later
  if (targetShotsFired) {
    int freeSpace = pulsesPerRound - targetShotUntil;
    int averageSpaceUsed = targetShotUntil / targetShotsFired;
    averageSpaceUsed += averageSpaceUsed / 10; //give it plus 10% for security
    if ((freeSpace - averageSpaceUsed) <= 0 ) {
      error("No free target space");
      return;
    }
  }

#ifndef DEBUG
  while ( targetPosition != (targetShotUntil) ); //pay attention to declare the variable as volatile otherwise the IDE optimizes the check out
#endif

  digitalWrite(triggerShutter, 0);
  delay(shutterOpeningTime);

  triggerCameras();

  delay(sequenceLength - (triggerPulseLength_us / 1000));
  digitalWrite(triggerShutter, 1);
  delay(shutterClosingTime);

  if (targetPosition < targetShotUntil){
    error("Target Overshot");
    targetShotUntil = pulsesPerRound;
    targetShotsFired++;
  }
  else{
    ok();
    targetShotUntil = targetPosition + 1;
    targetShotsFired++;
  }

}

float absToDeg(int pos){
  return degPerPulse * pos;
}


//command execution routine
void commandExecute(String command) {
  if (command.substring(0,6) == "shoot=") {
    int sequenceLength = command.substring(6).toInt();
    if((sequenceLength <= 0) || (sequenceLength > 2500)){
      error("SequenceLength out of range!");
      return;
    }
    shootingRoutine(sequenceLength); //shootingRoutine sends its own answer
  } 
  else if (command == "shotUntil") {
    Serial.println(absToDeg(targetShotUntil));
  } 
  else if (command == "resetTarget") {
    targetShotUntil = 0;
    ok();
  } 
  else if (command == "triggerCameras") {
    triggerCameras();
    ok();
  } 
  else if (command == "targetPosition") {
    Serial.println(absToDeg(targetPosition));
  } 
  else if (command == "calibrate") {
    targetPosition = 0;
    ok();
  } 
  else if (command == "motorStart") {
    melservoStart();
    ok();
  } 
  else if (command == "motorStop") {
    melservoStop();
    ok();
  } 
  else if (command == "testFunction") {
    digitalWrite(triggerFlipBlocker, 0);
    delay(2000);
    digitalWrite(triggerFlipBlocker, 1);
    ok();
  }else{
    error("Unknown Command!");
  }

}




// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  digitalWrite(trigger0, 0);
  digitalWrite(trigger1, 0);
  digitalWrite(trigger2, 0);
  digitalWrite(trigger3, 0);
  digitalWrite(trigger4, 0);
  pinMode(trigger0, OUTPUT);
  pinMode(trigger1, OUTPUT);
  pinMode(trigger2, OUTPUT);
  pinMode(trigger3, OUTPUT);
  pinMode(trigger4, OUTPUT);
  
  digitalWrite(openCollectorOutput0, 0);
  digitalWrite(openCollectorOutput1, 0);
  digitalWrite(openCollectorOutput2, 0);
  digitalWrite(openCollectorOutput3, 0);
  digitalWrite(openCollectorOutput4, 0);
  digitalWrite(openCollectorOutput5, 0);
  digitalWrite(openCollectorOutput6, 0);
  digitalWrite(openCollectorOutput7, 0);
  digitalWrite(openCollectorOutput8, 0);
  digitalWrite(openCollectorOutput9, 0);
  digitalWrite(openCollectorOutput10, 0);
  digitalWrite(openCollectorOutput11, 0);
  
  pinMode(openCollectorOutput0, OUTPUT);
  pinMode(openCollectorOutput1, OUTPUT);
  pinMode(openCollectorOutput2, OUTPUT);
  pinMode(openCollectorOutput3, OUTPUT);
  pinMode(openCollectorOutput4, OUTPUT);
  pinMode(openCollectorOutput5, OUTPUT);
  pinMode(openCollectorOutput6, OUTPUT);
  pinMode(openCollectorOutput7, OUTPUT);
  pinMode(openCollectorOutput8, OUTPUT);
  pinMode(openCollectorOutput9, OUTPUT);
  pinMode(openCollectorOutput10, OUTPUT);
  pinMode(openCollectorOutput11, OUTPUT);

  pinMode(motorPhaseAPin, INPUT_PULLUP);
  pinMode(motorPhaseBPin, INPUT_PULLUP);

  attachInterrupt(motorPhaseAInterrupt, interruptRoutine, FALLING);
  melservoInitialize();
  
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:

long timeLastWork = 0; //variables to do something every x ms 
const int timeWorkPeriod = 10;

const int commandBufferLength = 20;
char commandBuffer[commandBufferLength];
int commandBufferPosition = 0;
String command;

void loop()
{

  //do something every x miliseconds
  if ((millis() - timeLastWork) >= timeWorkPeriod) {
    timeLastWork += timeWorkPeriod;
    
#ifdef DEBUG
    targetPosition++;
    targetPosition = targetPosition % pulsesPerRound;
#endif



  }


  //receive incoming comunication and check if we have to execute a command
  if (Serial.available() != 0) {

    if (commandBufferPosition == commandBufferLength) {
      error("commandBuffer overflow");
      Serial.flush();
      commandBufferPosition = 0;
    } 
    else {
      commandBuffer[commandBufferPosition] = Serial.read();
    }

    switch (commandBuffer[commandBufferPosition]) {
    case '!':
    case '?':
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
  } 
  else {
    targetPosition--;
  }

  if (targetPosition >= pulsesPerRound) {
    targetPosition = targetPosition - pulsesPerRound;
    targetRotations++;
  } 
  else if (targetPosition < 0) {
    targetPosition = targetPosition + pulsesPerRound;
    targetRotations--;
  }

  targetPositionChanged = 1;
}



