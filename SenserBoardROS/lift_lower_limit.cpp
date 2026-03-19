#include <BluetoothSerial.h>
#include "globals.h"
#include <Arduino.h>



void lower_limit_setup() {
  pinMode(lowerlimitSwitchPin, INPUT_PULLUP);
  // Serial.begin(115200);
  // SerialBT.begin("ESP32_LimitSwitch");
  // Serial.println("Bluetooth Started. Waiting for limit switch...");
}

void lower_limit() {
  bool lcurrentState = digitalRead(lowerlimitSwitchPin);

  // Detect state change: pressed (from HIGH to LOW)
  if (llastState == HIGH && lcurrentState == LOW) {
    //Serial.println("Lower Limit switch pressed!");
    SerialBT.println("Lower Limit switch pressed!");
  }

  llastState = lcurrentState;
  delay(50);  // Debounce delay
}
