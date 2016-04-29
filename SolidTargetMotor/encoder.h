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
 
#ifndef ENCODER_H
#define ENCODER_H

class Encoder
{
  public:
    Encoder(int);
    ~Encoder();
    void tick();
    void sTick();
    float getSpeed();
    float getAngle();
    bool wait(float, int);
    void zero(void);

  private:
    int angleToTicks(float);
    float ticksToAngle(int);
    int rollover(int);
    
    volatile int ticks; //pay attention to declare the variable as volatile otherwise the IDE optimizes the check out
    int ticksPerRound;
    

};

#endif
