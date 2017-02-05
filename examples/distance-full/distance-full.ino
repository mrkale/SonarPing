/*
  NAME:
  Full usage of SonarPing library.

  DESCRIPTION:
  This sketch demonstrates the use of SonarPing for distance measuring.
  * The sketch can be run either on Arduino platform or Particle platform
    (Photon, Electron, P1).
  * The ambient air temperature for sound speed compensation is simulated
    by random numbers in the range 10 to 40 centigrades.
  * The measuring distance is limited to the range 5 - 250 cm.
  * Connect sensor's TRIG pin to microcontroller's digital pin 2.    
  * Connect sensor's ECHO pin to microcontroller's digital pin 3.    

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the MIT License (MIT).

  CREDENTIALS:
  Author: Libor Gabaj
*/
#include "sonar-ping.h"
#define SKETCH "DISTANCE-FULL 1.0.0"

const unsigned int PERIOD_MEASURE = 3000;      // Time in miliseconds between measurements

// Hardware cofiguration
const unsigned char PIN_TRIG = 2;
const unsigned char PIN_ECHO = 3;

// Measurement
const unsigned int DISTANCE_MIN = 5;
const unsigned int DISTANCE_MAX = 250;
const byte TEMPERATURE_MIN = 10;
const byte TEMPERATURE_MAX = 40;
SonarPing sonar(PIN_TRIG, PIN_ECHO, DISTANCE_MAX, DISTANCE_MIN);
unsigned int distanceValue;

void setup()
{
#if defined(ARDUINO)
  Serial.begin(9600);
  Serial.println(F(SKETCH));
  Serial.println(F(SONARPING_VERSION));
  Serial.print(F("Distance: "));
  Serial.print(F("Distance: "));
  Serial.print(DISTANCE_MIN);
  Serial.print(F(" ~ "));
  Serial.print(DISTANCE_MAX);
  Serial.print(F(" cm"));
  Serial.println();
  // Print header
  Serial.println(F("---"));
  Serial.println("Distance\tTemperature");
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
    // Temperature compensation
    sonar.setTemperature(random(TEMPERATURE_MIN, TEMPERATURE_MAX + 1));
    distanceValue = sonar.measureDistance();
    byte tempValue = sonar.getTemperature();
#if defined(ARDUINO)
    Serial.print(distanceValue);
    Serial.print(F("\t\t"));
    Serial.print(tempValue);
    Serial.println();      
#elif defined(PARTICLE)
    Particle.publish("Distance", String::format("%3d cm", distanceValue));
    Particle.publish("Temperature", String::format("%2d degC", tempValue));
#endif
  }
}
