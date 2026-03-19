#include "globals.h"
#include <Arduino.h>
unsigned long lastBlinkTime = 0;
bool lightState = false;
const unsigned long blinkInterval = 300;
void warning_switch_setup() {
  pinMode(switchPin, INPUT_PULLUP); 
  pinMode(LightPin, OUTPUT);
  digitalWrite(LightPin, LOW);
}
void warning_switch() {
  if (digitalRead(switchPin) == LOW) {
    robot_danger = false;
    robot_warning = false;
  } else {
    if (echo_danger || gyro_danger) {
      robot_danger = true;
      robot_warning = true;
    } else if (echo_warning || gyro_warning) {
      robot_danger = false;
      robot_warning = true;
    } else {
      robot_danger = false;
      robot_warning = false;
    }
  }
  if (robot_danger) {
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= blinkInterval) {
      lightState = !lightState;  // Toggle light
      digitalWrite(LightPin, lightState ? HIGH : LOW);
      lastBlinkTime = currentTime;
    }
  } 
  else if (robot_warning){
    digitalWrite(LightPin, HIGH);  // Solid ON during warning
  } 
  else if (movingmode == 2){
    digitalWrite(LightPin, HIGH);
  }
  else {
    digitalWrite(LightPin, LOW);   // OFF during normal operation
  }
}
