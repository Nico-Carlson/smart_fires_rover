#include "globals.h"
#include <BluetoothSerial.h>
BluetoothSerial SerialBT; 
int min_distance = 500;
int Roll_Angle = 0;
int Pitch_Angle = 0;
bool robot_danger = false;
bool robot_warning = false;
bool echo_danger = false;
bool echo_warning = false;
bool gyro_danger = false;
bool gyro_warning = false;
bool isOn = false;
String output = "";
const int lowerlimitSwitchPin = 13;
bool llastState = HIGH;
char c = '\0';
const int upperlimitSwitchPin = 14;
bool ulastState = HIGH;
bool lcurrentState = HIGH;
bool ucurrentState = HIGH;
int movingmode = 0;
//Turning angle
double turnangle = 360;
