/*
  NAME:
  Basic usage of SonarPing library.

  DESCRIPTION:
  This sketch demonstrates the use of SonarPing library for distance
  measuring with default arguments.
  * The sketch can be run either on Arduino platform or Particle platform
    (Photon, Electron, P1).
  * The ambient air temperature for sound speed compensation is defaulted
    to library default.
  * The measuring distance range is defaulted to library defaults.
  * Connect sensor's TRIG pin to microcontroller's digital pin 2.    
  * Connect sensor's ECHO pin to microcontroller's digital pin 3.    

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "sonar-ping.h"
#define SKETCH "DISTANCE-BASIC 1.0.0"

const unsigned int PERIOD_MEASURE = 3000;      // Time in miliseconds between measurements

// Hardware cofiguration
const unsigned char PIN_TRIG = 2;
const unsigned char PIN_ECHO = 3;

// Measurement
SonarPing sonar(PIN_TRIG, PIN_ECHO);
unsigned int distanceValue;

void setup()
{
#if defined(ARDUINO)
  Serial.begin(9600);
  Serial.println(F(SKETCH));
  Serial.println(F(SONARPING_VERSION));
  Serial.println(F("---"));
#elif defined(PARTICLE)
  Particle.publish("Sketch",  String(SKETCH));
  Particle.publish("Library", String(SONARPING_VERSION));
#endif
}

void loop()
{
  processing();
}

void processing()
{
  static unsigned long timeStamp;
  if (millis() - timeStamp >= PERIOD_MEASURE || timeStamp == 0)
  {
    timeStamp = millis();
    distanceValue = sonar.measureDistance();
#if defined(ARDUINO)
    Serial.print(F("Distance = "));
    Serial.print(distanceValue);
    Serial.print(F(" cm"));
    Serial.println();
#elif defined(PARTICLE)
    Particle.publish("Distance", String::format("%3d cm", distanceValue));
#endif
  }
}
