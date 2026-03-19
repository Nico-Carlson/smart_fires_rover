#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "globals.h"
#include <Arduino.h>
Adafruit_MPU6050 mpu;

void gyro_setup(){
  Serial.begin(115200);
  gyro_warning = false;
  gyro_danger = false;

  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  //Try to initialize!
 if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
 }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  // Serial.println("");
  // delay(100);
}
void gyro(){
  if (!mpu.begin()) {
    //Serial.println("Failed to find MPU6050 chip");
    echo_danger = false;
    echo_warning = false;
  } else {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  Roll_Angle = round( atan2 (a.acceleration.x, a.acceleration.z) * 180/3.14159265 );
  if (Roll_Angle > 0){
    Roll_Angle = ((Roll_Angle - 180) * -1);
  }
    else if (Roll_Angle < 0){
      Roll_Angle = (Roll_Angle + 180);
    }
  Pitch_Angle = ((round( atan2 (a.acceleration.y, a.acceleration.z) * 180/3.14159265 )) + 167);

  if (Pitch_Angle > 300){
    Pitch_Angle = ((((Pitch_Angle - 167) - 180) * -1) + 13);
  }
  if (Pitch_Angle < 0){
    Pitch_Angle = (Pitch_Angle * -1);
  }
  if (Roll_Angle >= 20 || Pitch_Angle >= 25){
    gyro_danger = true;
    gyro_warning = false;
  } else if (Roll_Angle >= 15 || Pitch_Angle >= 20){
    gyro_warning = true;
    gyro_danger = false ;
  } else{
    gyro_warning = false;
    gyro_danger = false;
  }
  }
}
