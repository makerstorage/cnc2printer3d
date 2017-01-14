#include "Arduino.h"
#include <Servo.h>
#include <digitalWriteFast.h>  // library for high performance reads and writes by jrraines
                               // see http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1267553811/0
                               // and http://code.google.com/p/digitalwritefast/
 
// It turns out that the regular digitalRead() calls are too slow and bring the arduino down when
// I use them in the interrupt routines while the motor runs at full speed creating more than
// 40000 encoder ticks per second per motor.
 
// Quadrature encoders
// Left encoder
#define c_LeftEncoderInterrupt 0
#define c_LeftEncoderPinA 2
#define c_LeftEncoderPinB 3 //25
//#define LeftEncoderIsReversed
//volatile bool _LeftEncoderBSet;
//volatile long _LeftEncoderTicks = 0;

//stepper motor pins
#define stepPin 18
#define dirPin 19 //25

bool stepState;

void setup()
{
 // Serial.begin(115200);
 

  // Quadrature encoders
  // Left encoder
  pinMode(c_LeftEncoderPinA, INPUT);      // sets pin A as input
  digitalWrite(c_LeftEncoderPinA, LOW);  // turn on pullup resistors
  pinMode(c_LeftEncoderPinB, INPUT);      // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW);  // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterrupt, HandleLeftMotorInterruptA, RISING);

  //Stepper Motor
  pinMode(stepPin, OUTPUT);      // sets steppin  as output
  pinMode(dirPin, OUTPUT);      // sets dirpin  as output



}
 
void loop()
{
 
 
//  Serial.print(_LeftEncoderTicks);
//  Serial.print("\t");
  
//  Serial.print(_LeftEncoderBSet);
  
//  Serial.print("\n");
  
 
  delay(80);
}
 
// Interrupt service routines for the left motor's quadrature encoder
void HandleLeftMotorInterruptA()
{
 
  digitalWriteFast(dirPin,digitalReadFast(c_LeftEncoderPinB));   

  digitalWriteFast(stepPin,stepState);
  stepState = !stepState; 

}
 

