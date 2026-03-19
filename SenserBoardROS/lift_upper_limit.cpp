#include <BluetoothSerial.h>
#include "globals.h"
#include <Arduino.h>



void upper_limit_setup() {
  pinMode(upperlimitSwitchPin, INPUT_PULLUP);
  // Serial.begin(115200);
  // SerialBT.begin("ESP32_LimitSwitch");
  // Serial.println("Bluetooth Started. Waiting for limit switch...");
}

void upper_limit() {
  bool ucurrentState = digitalRead(upperlimitSwitchPin);

  // Detect state change: pressed (from HIGH to LOW)
  if (ulastState == HIGH && ucurrentState == LOW) {
    //Serial.println("Upper Limit switch pressed!");
    SerialBT.println("Upper Limit switch pressed!");
  }

  ulastState = ucurrentState;
  delay(5);  // Debounce delay
}
