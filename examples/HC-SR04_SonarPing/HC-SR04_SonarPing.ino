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
#define SKETCH "HC-SR04_SonarPing 1.2.0"

// Pins for ultrasonic sensor connection
const byte PIN_TRIGGER = 2;
const byte PIN_ECHO = 3;

SonarPing sonar(PIN_TRIGGER, PIN_ECHO);

void setup()
{
  Serial.begin(9600);
  Serial.println(F(SKETCH));
  Serial.println(F(SONARPING_VERSION));
  Serial.println(F("Distance in cm:"));
  // Set ambient temperature (should be located in loop for real measurement)
  sonar.setTemperature(25);
}

void loop()
{
  Serial.println(sonar.getDistance());
  delay(1000);
}