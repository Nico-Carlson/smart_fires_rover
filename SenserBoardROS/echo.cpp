#include "globals.h"
#include <Arduino.h>
#include <BluetoothSerial.h>

const int trigPin = 33;
const int frontechoPin[] = {25, 26, 27}; 
const int backechoPin[] = {18, 5}; 

#define SOUND_SPEED 0.0133858
#define PULSE_TIMEOUT 10000

long duration;
float distanceInch;

void echo_setup() {
  pinMode(trigPin, OUTPUT);

  // Initialize front echo pins
  for (int i = 0; i < sizeof(frontechoPin)/sizeof(frontechoPin[0]); i++) {
    pinMode(frontechoPin[i], INPUT);
  }

  // Initialize back echo pins
  for (int i = 0; i < sizeof(backechoPin)/sizeof(backechoPin[0]); i++) { // skip index 0
    pinMode(backechoPin[i], INPUT);
  }

  echo_danger = false;
  echo_warning = false;
  min_distance = 500;
}

void echo() {
  // Set danger/warning flags
  if (min_distance < 15) {
    echo_danger = true;
    echo_warning = false;
  } else if (min_distance < 32) {
    echo_danger = false;
    echo_warning = true;
  } else {
    echo_danger = false;
    echo_warning = false;
  }

  min_distance = 500;

  const int* activePins;
  int numSensors;

  // Select front or back based on movingmode
  if (movingmode == 1) {
    activePins = frontechoPin;
    numSensors = sizeof(frontechoPin) / sizeof(frontechoPin[0]);
  } 
  if (movingmode == 2) {
    activePins = backechoPin;
    numSensors = (sizeof(backechoPin) / sizeof(backechoPin[0])) - 1;
  }

  // Loop over selected sensors
  for (int i = 0; i < numSensors; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);      
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);      
    digitalWrite(trigPin, LOW);

    duration = pulseIn(activePins[i], HIGH, PULSE_TIMEOUT);
    distanceInch = (duration * SOUND_SPEED) / 2.0;

    if (distanceInch < min_distance && distanceInch > 1) {
      min_distance = distanceInch;
    }

    if (c == 'e') {
      SerialBT.print("Distance ");
      SerialBT.print(i);
      SerialBT.print(": ");
      SerialBT.print(distanceInch);
      SerialBT.println(" inches");
    }

    delay(5);
  }
}
