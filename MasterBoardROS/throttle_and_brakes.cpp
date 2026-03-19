#include "globals.h"
#include <Arduino.h>

// Control pins
// const int lpwmPin = 18;  // LPWM pin
// const int rpwmPin = 21;  // RPWM pin

volatile unsigned long throttleStart = 0;

// new interrupt logic
void IRAM_ATTR throttle_isr() {
  if (digitalRead(pwmInputPin) == HIGH) {
    throttleStart = micros();
  } else {
    throttlePulse = micros() - throttleStart;
  }
}

bool hasExtended = false;
bool isRetracting = false;
unsigned long retractStartTime = 0;
const unsigned long retractDuration = 2000;



void throttle_and_brakes_setup() {
  pinMode(pwmInputPin, INPUT);

  // attach Interrupt to the pin
  attachInterrupt(digitalPinToInterrupt(pwmInputPin), throttle_isr, CHANGE);

  pinMode(dacOutputPin, OUTPUT);

  pinMode(hBridgePin1, OUTPUT); // L_EN
  pinMode(hBridgePin2, OUTPUT); // R_EN

  pinMode(lpwmPin, OUTPUT);
  pinMode(rpwmPin, OUTPUT);

  pinMode(limitSwitchPin, INPUT_PULLUP);

  digitalWrite(hBridgePin1, HIGH); // Enable left side
  digitalWrite(hBridgePin2, HIGH); // Enable right side
}

void setMotorForward() {
  analogWrite(lpwmPin, 255);  // Full speed forward
  analogWrite(rpwmPin, 0);
}

void setMotorBackward() {
  analogWrite(lpwmPin, 0);
  analogWrite(rpwmPin, 255);  // Full speed reverse
}

void stopMotor() {
  analogWrite(lpwmPin, 0);
  analogWrite(rpwmPin, 0);
}

void throttle_and_brakes() {
  unsigned long currentPulse = throttlePulse;
  unsigned long safeTurnSwitch = turnSwitchPulse; // The master RC override switch

  if (robot_danger) {
    dacWrite(dacOutputPin, 0); // Hard stop
  }
  else if (safeTurnSwitch > 1400) {
    // --- AI AUTONOMOUS MODE ---
    dacValue = 0;

    // Nav2 outputs speed in meters per second (e.g., 0.0 to 1.5 m/s).
    // We multiply by 100 so we can map integers (0 to 150) safely.
    if (forward_speed > 0.05) { // Small deadband so it doesn't creep
      dacValue = map(forward_speed * 100, 0, 150, 0, dacMax); // can reduce 150 to be lower if rover is crawling slowly
      dacValue = constrain(dacValue, 0, dacMax);
    }

    if (robot_warning) {
      dacValue = (dacValue * 0.75); // 25% speed reduction in warning zone
    }

    dacWrite(dacOutputPin, dacValue);

    // Optional: Print to Serial BT for debugging
    // Serial.print("AI Target (m/s): "); Serial.println(forward_speed);
  }
  else {
    // --- MANUAL RC MODE ---
    dacValue = 0;

    // Your inverted manual throttle mapping
    if (currentPulse >= pwmMin && currentPulse <= pwmMax) {
      dacValue = map(currentPulse, pwmMin, pwmMax, dacMax, 0);
    }

    if (robot_warning) {
      dacValue = (dacValue * 0.75);
    }

    dacWrite(dacOutputPin, dacValue);
  }

  // --- Linear Actuator Logic (Remains Unchanged) ---
  if (currentPulse > 2000) {
    if (digitalRead(limitSwitchPin) == HIGH) {
      setMotorForward();
      hasExtended = true;
      isRetracting = false;
    } else {
      stopMotor();
    }
  } else {
    if (hasExtended && !isRetracting) {
      setMotorBackward();
      retractStartTime = millis();
      isRetracting = true;
    } else if (isRetracting) {
      if (millis() - retractStartTime >= retractDuration) {
        stopMotor();
        isRetracting = false;
        hasExtended = false;
      } else {
        setMotorBackward();
      }
    } else {
      stopMotor();
    }
  }
}

