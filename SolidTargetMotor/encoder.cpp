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
 
  Encoder::Encoder(unsigned int ticksPerRoundInit){
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
    unsigned int tickToWaitFor = angleToTicks(angle);
    while ( ticks != tickToWaitFor );
  
  }
  unsigned int Encoder::angleToTicks(float angle){
    return (unsigned int) (angle / 360 * ticksPerRound);
  }
  
  float Encoder::ticksToAngle(unsigned int ticksValue){
    return 360.0 / ticksPerRound * ticksValue;
  }
  
  unsigned int Encoder::rollover(unsigned int ticksValue){
    
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

 
