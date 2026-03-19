#include <BluetoothSerial.h>
#include "globals.h"
#include <Arduino.h>


void bluetooth_setup() {
  SerialBT.begin("Rover"); // Bluetooth name
  analogReadResolution(12);
  c = '\0';
}

void bluetooth() {

   if (SerialBT.available()) {
    char incoming = SerialBT.read();
    if(incoming != '\n' && incoming != '\r'){
      c = incoming;
    }
    //Serial.print("New command received: ");
    //Serial.println(c);
   }
     bool currentState = digitalRead(lowerlimitSwitchPin);

  // lift lower limit switch
  if (llastState == HIGH && lcurrentState == LOW) {
    //Serial.println("Lower Limit switch pressed!");
    SerialBT.println("Lower Limit switch pressed!");
    }

// lift upper limit switch
  if (ulastState == HIGH && ucurrentState == LOW) {
    //Serial.println("Upper Limit switch pressed!");
    SerialBT.println("Upper Limit switch pressed!");
    }
     if (c == 'e') { 
      SerialBT.print("closes distance: ");
      SerialBT.println(min_distance);
    } else if (c == 'g') { 
      SerialBT.print("Roll: ");
      SerialBT.println(Roll_Angle);
      SerialBT.println(" ");
      SerialBT.print("Pitch: ");
      SerialBT.println(Pitch_Angle);
    } else if (c == 'd') { 
      SerialBT.print("robot_danger: ");
      SerialBT.println(robot_danger);
      SerialBT.print("robot_warning: ");
      SerialBT.println(robot_warning);
    } else if (c == 'b') { 
      SerialBT.println(output);
    }
  }


