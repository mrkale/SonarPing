/*
  NAME:
  Distance measuring statistically smoothed

  DESCRIPTION:
  This sketch demonstrates the use of ultrasonic distance measurement with
  smoothing drifts in data by statistical processing with library
  SmoothSensorData.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "SonarPing.h"
#include "SmoothSensorData.h"
#define SKETCH "HC-SR04_Smoothing 1.2.0"

// Pins for ultrasonic sensor connection
const byte PIN_TRIGGER = 2;
const byte PIN_ECHO = 3;

SonarPing sonar(PIN_TRIGGER, PIN_ECHO);
SmoothSensorData samples = SmoothSensorData();

void setup()
{
  Serial.begin(9600);
  Serial.println(F(SKETCH));
  Serial.println(F(SONARPING_VERSION));
  // Print header
  Serial.print(F("Median from readings: "));
  Serial.println(samples.getBufferLen());
  Serial.println(F("Distance in cm:"));
}

void loop()
{
  while (samples.registerData(sonar.getDistance())); // Readings to buffer
  Serial.println(samples.getMedian());  // Smoothing
  delay(1000);
}