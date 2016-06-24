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
#define SKETCH_VERSION "1.1.0"

// Pins for ultrasonic sensor connection
const byte pinTrigger = 2;
const byte pinEcho = 3;

SonarPing sonar(pinTrigger, pinEcho);
SmoothSensorData samples = SmoothSensorData();

void setup()
{
  Serial.begin(9600);
  Serial.print(F("Sketch v"));
  Serial.println(SKETCH_VERSION);
  Serial.print(F("SonarPing v"));
  Serial.println(SONARPING_VERSION);
  // Print header
  Serial.print(F("Median from readings: "));
  Serial.println(samples.getBufferLen());
  Serial.println("Distance in cm:");
}

void loop()
{
  while (samples.registerData(sonar.getDistance())); // Readings to buffer
  Serial.println(samples.getMedian());  // Smoothing
  delay(1000);
}