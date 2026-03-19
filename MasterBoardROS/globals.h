#ifndef GLOBALS_H
#define GLOBALS_H

extern bool robot_danger;
extern bool robot_warning;
extern bool echo_danger;
extern bool echo_warning;
extern bool gyro_danger;
extern bool gyro_warning;
extern int isOn;

extern int dacValue;
extern int pulseWidth;

extern double forward_speed;

// replaces the need for pulseIn() so that the pi doesnt think esps are stalling
extern volatile unsigned long throttlePulse;
extern volatile unsigned long reversePulse;
extern volatile unsigned long turnSwitchPulse;
extern volatile unsigned long steeringPulse;
extern volatile unsigned long liftPulse;


const int dacMax = 255;

//reverse (receiver channel 7 to pin 34; pin 23 to relay input pin)
const int revpwmInputPin = 34;
const int relayOutputPin = 23;

//throttle (receiver channel 2 to pin 32; pin 25 to throttle input (green wire-blue wire goes to ground))
//brakes (pin 19 to H-bridge input 1; pin 14 to H-bridge input pin 2; pin 15 to H-bridge LPWM; pin 0 to H-bridge RPWM; pin 13 to built-in brake limit switch)
const int pwmInputPin = 32;
const int dacOutputPin = 25;
const int hBridgePin1 = 14;
const int hBridgePin2 = 19;
const int limitSwitchPin = 13;
const int lpwmPin = 18;
const int rpwmPin = 2;
//PWM range for throttle
const int pwmMin = 980;
const int pwmMax = 1517;

//lift (receiver channel 6 to pin 4; pin 26 to lift input 1; pin 27 to lift input 2)
const int pwmliftInputPin = 4;
const int outputPinA = 26;
const int outputPinB = 27;
//PWM range for lift control switch
const int lowerThreshold = 1400;
const int upperThreshold = 1800;

// turning pins
const int rcinputpin  = 35;   // Radio input
const int servopin    = 33;   // Servo output
const int turnswitch = 22;  // make sure this is set to a switch on the controller
//waypoint stuff
extern double turnangle;

//I think this is old code that got left in
//const int relayPin = 2;
//const int switchPin = 17;

#endif
