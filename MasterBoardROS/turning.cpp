#include <ESP32Servo.h>
#include <Arduino.h>
#include "globals.h"

Servo steering;

// new Interrupt Logic
volatile unsigned long turnSwitchStart = 0;
volatile unsigned long steeringStart = 0;

void IRAM_ATTR turnswitch_isr() {
  if (digitalRead(turnswitch) == HIGH) {
    turnSwitchStart = micros();
  } else {
    turnSwitchPulse = micros() - turnSwitchStart;
  }
}

void IRAM_ATTR steering_isr() {
  if (digitalRead(rcinputpin) == HIGH) {
    steeringStart = micros();
  } else {
    steeringPulse = micros() - steeringStart;
  }
}

void turning_setup() {
  pinMode(turnswitch, INPUT);
  pinMode(rcinputpin, INPUT);

  // attach the hardware interrupts
  attachInterrupt(digitalPinToInterrupt(turnswitch), turnswitch_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rcinputpin), steering_isr, CHANGE);

  steering.setPeriodHertz(50);
  steering.attach(servopin, 1000, 2000);  // better range
}

void turning() {
  // read the background variables
  unsigned long safeTurnSwitch = turnSwitchPulse;
  unsigned long safeRCInput = steeringPulse;
  
  if (safeTurnSwitch > 1400) {
    float turn = constrain(turnangle, -30.0, 30.0);
    
    // Expanded range for the Dorky 3 servo (500 to 2500)
    int minPulse = 500; 
    int maxPulse = 2500;

    int outputPulse = minPulse + ((turn + 30.0) / 60.0) * (maxPulse - minPulse);
    steering.writeMicroseconds(outputPulse);
  }
  else {

    // Standard RC receivers output 1000-2000, 
    // but we can scale this up if the servo feels limited:
    if (safeRCInput >= 900 && safeRCInput <= 2100) {
       // Optional: Map the 1000-2000 RC signal to the 500-2500 Servo range
       int boostedPulse = map(safeRCInput, 1000, 2000, 500, 2500);
       steering.writeMicroseconds(boostedPulse);
    }
  }
}
