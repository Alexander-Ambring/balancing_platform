/*
  File: balancing_platform.ino
  Author: Alexander Ambring
  Date: 2023-11-27
  Description: This program uses servo motors to automatically balance a platform.
*/

// Gets and includes necessary libraries
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>


// Initializing necessary objects and variables
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
Servo servo8;
Servo servo9;

float acc_x;
float acc_y;
float acc_z;
int angle_x;
int angle_y;
int valuesX[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int valuesY[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float meanX;
float meanY;

void setup(){
  // Attaching servos to specific pins and starting the serial monitor
  servo8.attach(8);
  servo9.attach(9);
  Serial.begin(9600);

  // Warns in case the accelerometer does not start
  if (!accel.begin()){
    Serial.println("No ADXL345 sensor detected.");
    while (1);
  }
}

void loop(){
  updateServo();

  // Reading accelerometer values
  sensors_event_t event;
  accel.getEvent(&event);

  acc_x = event.acceleration.x;
  acc_y = event.acceleration.y;
  acc_z = event.acceleration.z;

  // Calculating angles based on accelerometer readings and mapping them to match the servos 0-180 degrees
  angle_x = constrain(map(arctangent(acc_z, acc_y) - 3.5, -90, 90, 0, 180), 0, 180);
  angle_y = constrain(map(arctangent(acc_z, acc_x) - 3.5, -90, 90, 0, 180), 0, 180);

  // Updating arrays and calculating means
  getNewValueX();
  getNewValueY();
  getNewMeanX();
  getNewMeanY();

  delay(20);
}

// Updates servo positions based on mean values
void updateServo(){
  servo8.write(meanX);
  servo9.write(meanY);
}

/*
 Calculating the arctangent of two values
 Parameters:
 - a: The first value
 - b: The second value
 Returns: The arctangent in degrees
 */
float arctangent(float a, float b){
  float degree;
  degree = atan2(b, a) * 57.2957795131;
  return degree;
}

// Shifting values in the X array and adding a new value
void getNewValueX(){
  for (int i = 9; i > 0; --i){
    valuesX[i] = valuesX[i - 1];
  }
  valuesX[0] = angle_x;
} 

// Shifting values in the Y array and adding a new value
void getNewValueY(){
  for (int i = 9; i > 0; --i){
    valuesY[i] = valuesY[i - 1];
  }
  valuesY[0] = angle_y;
}

// Calculating mean value for X array
void getNewMeanX(){
  float sumX = 0;
  for (int i = 0; i < 10; ++i){
    sumX += valuesX[i];
  }
  meanX = sumX / 10;
}

// Calculating mean value for Y array
void getNewMeanY(){
  float sumY = 0;
  for (int i = 0; i < 10; ++i){
    sumY += valuesY[i];
  }
  meanY = sumY / 10;
}
