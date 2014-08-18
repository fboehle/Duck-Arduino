/**********************************************************
 *	Class to calculate the position and speed from motor encoder pulses
 *
 *	Developement started: 07/2014
 *	Author: Frederik Böhle code@fboehle.de
 *
 **********************************************************
 *
 *   Description: 
 *
 *   Notes:
 *
 *   Changelog:
 *
 **********************************************************/ 
 
 #include "encoder.h"
 
  Encoder::Encoder(int ticksPerRoundInit){
    ticksPerRound = ticksPerRoundInit;
    ticks = 0;
  }
  Encoder::~Encoder(){
    //
  }
  void Encoder::tick(){
    ticks = rollover(ticks+1);
  }
  void Encoder::sTick(){
    ticks = rollover(ticks-1);
  }
  void Encoder::zero(void){
    ticks = 0;
  }
  float Encoder::getSpeed(){
    //
  }
  float Encoder::getAngle(){
    return ticksToAngle(ticks);
  }
  bool Encoder::wait(float angle){
    int tickToWaitFor = angleToTicks(angle);
    while ( ticks != tickToWaitFor );
  
  }
  int Encoder::angleToTicks(float angle){
    return rollover((int) (angle / 360 * ticksPerRound));
  }
  
  float Encoder::ticksToAngle(int ticksValue){
    return 360.0 / ticksPerRound * ticksValue;
  }
  
  int Encoder::rollover(int ticksValue){
    
    if (ticksValue >= ticksPerRound) {
      return ticksValue - ticksPerRound;
    } 
    else if (ticksValue < 0) {
      return ticksValue + ticksPerRound;
    }
    else{
      return ticksValue;
    }
 
  }

 
