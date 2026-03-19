#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h> 
#include <BluetoothSerial.h>
#include <TinyGPS++.h>
#include <math.h>
#include <SD.h>
#include <SPI.h>

extern BluetoothSerial SerialBT;
extern int min_distance;
extern int Roll_Angle;
extern int Pitch_Angle;
extern bool robot_danger;
extern bool robot_warning;
extern bool echo_danger;
extern bool echo_warning;
extern bool gyro_danger;
extern bool gyro_warning;
extern bool isOn;
extern String output;
extern const int lowerlimitSwitchPin;
extern bool llastState;
extern const int upperlimitSwitchPin;
extern bool ulastState;
extern bool lcurrentState;
extern bool ucurrentState;
extern int movingmode;

extern char c;

const int voltagePin = 34; 

const int limitSwitchPin = 13;

const int pwmMin = 980;
const int pwmMax = 1517;


const int lowerThreshold = 1400;
const int upperThreshold = 1800;

const int LightPin = 19;
const int switchPin = 23;


// Calibration Values
const float adcMax = 4095.0;
const float refVoltage = 3.3;
const float fullChargeBattery = 39.8;
const float fullChargeDivider = 2.39;
const float voltageScale = fullChargeBattery / fullChargeDivider; 

//Turning angle
extern double turnangle;

//GPS Pins
#define RXD2 16
#define TXD2 17
#define GPS_BAUD 9600

// SD Card SPI Pins
#define SD_CS   5
#define SD_MOSI 23
#define SD_CLK  18
#define SD_MISO 19
 

#endif

