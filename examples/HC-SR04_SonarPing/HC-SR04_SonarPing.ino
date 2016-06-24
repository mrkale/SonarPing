/*
  NAME:
  Basic usage of SonarPing library

  DESCRIPTION:
  This sketch demonstrates the use of SonarPing for distance measuring
  with temperature compensation of the sound speed.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "SonarPing.h"
#define SKETCH_VERSION "1.1.0"

// Pins for ultrasonic sensor connection
const byte pinTrigger = 2;
const byte pinEcho = 3;

SonarPing sonar(pinTrigger, pinEcho);

void setup()
{
  Serial.begin(9600);
  Serial.print(F("Sketch v"));
  Serial.println(SKETCH_VERSION);
  Serial.print(F("SonarPing v"));
  Serial.println(SONARPING_VERSION);
  Serial.println("Distance in cm:");
  // Set ambient temperature (should be located in loop for real measurement)
  sonar.setTemperature(25);
}

void loop()
{
  Serial.println(sonar.getDistance());
  delay(1000);
}