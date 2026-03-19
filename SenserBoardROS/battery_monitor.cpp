#include <BluetoothSerial.h>
#include "globals.h"
#include <Arduino.h>

void battery() {
  int adcValue = analogRead(voltagePin);
  float measuredVoltage = (adcValue / adcMax) * refVoltage;
  float batteryVoltage = measuredVoltage * voltageScale;

  output = "ADC: " + String(adcValue) +
                  " | Measured: " + String(measuredVoltage, 2) + "V" +
                  " | Battery: " + String(batteryVoltage, 2) + "V";
}