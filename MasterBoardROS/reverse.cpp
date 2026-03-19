#include "globals.h"
#include <Arduino.h>

bool relayState = false;
unsigned long lastRelayToggle = 0; // new timer for non-blocking delay

// new Interrupt Logic
volatile unsigned long reverseStart = 0;

void IRAM_ATTR reverse_isr() {
  if (digitalRead(revpwmInputPin) == HIGH) {
    reverseStart = micros();
  } else {
    reversePulse = micros() - reverseStart;
  }
}

void reverse_setup(){
  pinMode(revpwmInputPin, INPUT);

  // attach the hardware interrupt
  attachInterrupt(digitalPinToInterrupt(revpwmInputPin), reverse_isr, CHANGE);

  pinMode(relayOutputPin, OUTPUT);
  digitalWrite(relayOutputPin, LOW);
}

void toggleRelay() {
  relayState = !relayState;
  digitalWrite(relayOutputPin, relayState ? HIGH : LOW);
}

void reverse() {
  if (robot_danger == false) {
    // read the background variable
    int pwmValue = reversePulse;

    // use millis() to enforce the 500ms cooldown without blocking
    if (pwmValue > 2000 && (millis() - lastRelayToggle > 500)) {
      toggleRelay();
      lastRelayToggle = millis(); // reset the cooldown timer

      Serial.print("PWM Detected: ");
      Serial.print(pwmValue);
      Serial.print(" µs - Relay is now ");
      Serial.println(relayState ? "ON" : "OFF");
    }
  } else {
    dacWrite(dacOutputPin, 0);
  }
}
