#include "globals.h"
#include <Arduino.h>

// new Interrupt Logic
volatile unsigned long liftStart = 0;

void IRAM_ATTR lift_isr() {
  if (digitalRead(pwmliftInputPin) == HIGH) {
    liftStart = micros();
  } else {
    liftPulse = micros() - liftStart;
  }
}

void lift_setup() {
  pinMode(pwmliftInputPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(pwmliftInputPin), lift_isr, CHANGE);

  pinMode(outputPinA, OUTPUT);
  pinMode(outputPinB, OUTPUT);
  Serial.println("ESP32: RC PWM to Digital Output on GPIO 26 & 27");
}

void lift() {
  // read the background variable
  unsigned long currentPulse = liftPulse;

  if (currentPulse == 0) {
    Serial.println("No PWM signal detected.");
    digitalWrite(outputPinA, HIGH);
    digitalWrite(outputPinB, HIGH);
    return;
  }

  if (robot_danger == false) {
    if (currentPulse > upperThreshold && dacValue == 0) {
      digitalWrite(outputPinA, HIGH);
      digitalWrite(outputPinB, LOW);
    } else if (currentPulse < lowerThreshold && dacValue == 0) {
      digitalWrite(outputPinA, LOW);
      digitalWrite(outputPinB, HIGH);
     } else {
        digitalWrite(outputPinA, HIGH);
        digitalWrite(outputPinB, HIGH);
        }
  } else {
    digitalWrite(outputPinA, HIGH);
    digitalWrite(outputPinB, HIGH);
  }
}
