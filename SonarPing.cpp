/*
  NAME:
  SonarPing

  DESCRIPTION:
  Library for ultrasonic sensor HC-SR04.
  - Library is inspired by the Tim Eckel's library NewPing version 1.5
    and 1.6 beta.
  - Library offers these functionalities:
    * Temperature compensation to sound speed.
    * Digital filtering with interquartile mean, median, and moving mean.
    * Expected measurement range (min. ~ max.).
    * Only SI measurement units.
      The conversion to imperial units should be provided in a sketch code
      or in a separate library in order to avoid code duplicities in sketches
      using multiple libraries with the same conversion functionalities.
  - Due to practical precision of the sensors the distances are allways 
    expressed in whole centimeters.
  - Temperature compensation is limited to the range -128°C to +127°C, which
    contains usual working range of the sensors.
  - Sound speed in meters per second with temperature correction is calculated
    from the statement
    soundSpeed = 331.3 + 0.6 * Centigrades

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
  (related to original code) and MIT License (MIT) for added code.
  
  CREDITS:
  Tim Eckel - teckel@leethost.com
  http://code.google.com/p/arduino-new-ping/
    
  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkale/SonarPing.git
  Version: 1.0.0
  Updated: 20.03.2015
 */
#include "SonarPing.h"

//-------------------------------------------------------------------------
// SonarPing constructors
//-------------------------------------------------------------------------
SonarPing::SonarPing(uint8_t trigger_pin, uint8_t echo_pin, \
                 uint16_t max_cm_distance, \
                 uint16_t min_cm_distance) {
  // Bitmask for pins
	_triggerBit = digitalPinToBitMask(trigger_pin);
	_echoBit = digitalPinToBitMask(echo_pin);
  // Data Direction Registers (DDR - Input/Output)
	_triggerOutput = portOutputRegister(digitalPinToPort(trigger_pin));
	_echoInput = portInputRegister(digitalPinToPort(echo_pin));
  // Port Mode Register for the trigger pin (PORT - HIGH/LOW)
	_triggerMode = (uint8_t *) portModeRegister(digitalPinToPort(trigger_pin));
  // Maintain input parameters
  _minDistance = constrain(min_cm_distance, LIMIT_MIN_DISTANCE, LIMIT_MAX_DISTANCE);
  _maxDistance = constrain(max_cm_distance, _minDistance, LIMIT_MAX_DISTANCE);
}
SonarPing::SonarPing(uint8_t trigger_pin, \
                 uint16_t max_cm_distance, \
                 uint16_t min_cm_distance) {
  SonarPing(trigger_pin, trigger_pin, max_cm_distance, min_cm_distance);
}

// ------------------------------------------------------------------------
// Standard ping methods
// ------------------------------------------------------------------------

// Measure sound ping round trip time (microseconds) to reflector
uint16_t SonarPing::ping_us() {
  uint32_t deadline;
  uint16_t pingTime = distance2time(_maxDistance);
  // Wait necessary time from potential previous ping (from specs)
  delay(INTERPING_DELAY);
  // Broadcast sound ping
	*_triggerMode |= _triggerBit;    // Set trigger pin to OUTPUT
	*_triggerOutput &= ~_triggerBit; // Set the trigger pin to LOW
  delayMicroseconds(4);            // Wait for pin to go LOW (from testing)
	*_triggerOutput |= _triggerBit;  // Set trigger pin to go HIGH - send ping
	delayMicroseconds(10);           // Wait for trigger pin become HIGH - from specs
	*_triggerOutput &= ~_triggerBit; // Set trigger pin to go LOW
  *_triggerMode &= ~_triggerBit;   // Set trigger pin to INPUT for one pin mode
  // Wait for start of measuring echo pulse
	deadline =  micros() + MAX_SENSOR_DELAY;  // Deadline for pulse start
	while (*_echoInput & _echoBit && micros() <= deadline){} // Wait for pin to go LOW
	while (!(*_echoInput & _echoBit)) {         // Wait for pin to go HIGH - start pulse
		if (micros() > deadline) return NULL;  // Pulse start failed
  }
  //  Expected maximal pulse stop
	deadline = micros() + pingTime;
  // Wait for end of measuring echo pulse
	while (*_echoInput & _echoBit) {
    // Pulse unexpectably long
		if (micros() > deadline) return NULL;
  }
  // Calculate ping
  pingTime = micros() - (deadline - pingTime);
  // Pulse is unexpectably short
  if (pingTime < distance2time(_minDistance)) return NULL;
  //Return correct ping time
	return pingTime;
}
// Measure distance
uint16_t SonarPing::distance() {
  return ping_us() / soundPace() + 0.5;
}
// Intequartile mean for distance
uint16_t SonarPing::distance_iqmean(uint8_t samples) {
  return calc_iqmean(&SonarPing::distance, samples);
}
// Median for distance
uint16_t SonarPing::distance_median(uint8_t samples) {
  return calc_median(&SonarPing::distance, samples);
}
// Store current temperature for further calculations
void SonarPing::setTemperature(int8_t temperature) {
  _temperature = temperature;
}
// Calculate moving mean
uint16_t SonarPing::calc_mvmean(uint16_t currentValue) {
  static uint16_t movingMean = NULL;
  if (movingMean == NULL) movingMean = currentValue;
  else movingMean = (movingMean + currentValue) / 2;
  return movingMean;
}

//-------------------------------------------------------------------------
// Private auxilliary methods
//-------------------------------------------------------------------------
// Sound pace corrected by temperature
uint8_t SonarPing::soundPace(void) {
  return (2000000.0 / (33130 + 60 * _temperature));
}
uint16_t SonarPing::distance2time(uint16_t distance) {
  return distance * 2 * soundPace();
}
// Calculate interquarter arithmetic mean
uint16_t SonarPing::calc_iqmean(uint16_t (SonarPing::*method)(void), uint8_t samples) {
  // Maintain input parameters  
  if (!samples) return NULL;  // For zero samples no processing
  // Align samples to the nearest multiple of 4 it is not already
  if (samples & 0x03) samples = ((samples + 4) & ~0x03);
  uint16_t sampleList[samples], sampleValue = NULL;
  uint8_t samplePos, sampleCnt = 0;
  while (sampleCnt < samples) {
    // Call input measurement method of this class
    sampleValue = (this->*method)();
    if (sampleValue == NULL) continue; // No valid sample value
    // Sort sample list from smallest to biggest value and make place in it
    // for current sample insertion
    for (samplePos = sampleCnt; samplePos > 0 && sampleList[samplePos - 1] > sampleValue; samplePos--) {
					sampleList[samplePos] = sampleList[samplePos - 1];
    }
    sampleList[samplePos] = sampleValue;
    sampleCnt++;
  };
  // Calculate intequarter arithmetic mean
  sampleCnt = samples / 2;
  samplePos = samples / 4;
  sampleValue = 0;
  for (uint8_t i = 0; i < sampleCnt; i++) {
    sampleValue += sampleList[samplePos + i];
  }
  return (sampleValue + 0.5 * sampleCnt) / sampleCnt;
}
// Calculate median
uint16_t SonarPing::calc_median(uint16_t (SonarPing::*method)(void), uint8_t samples) {
  // Maintain input parameters  
  if (!samples) return NULL;  // For zero samples no processing
  // Update samples to odd number
  samples |= 0x01;
  uint16_t sampleList[samples], sampleValue = NULL;
  uint8_t samplePos, sampleCnt = 0;
  while (sampleCnt < samples) {
    // Call input measurement method of this class
    sampleValue = (this->*method)();
    if (sampleValue == NULL) continue; // No valid sample value
    // Sort sample list from smallest to biggest value and make place in it
    // for current sample insertion
    for (samplePos = sampleCnt; samplePos > 0 && sampleList[samplePos - 1] > sampleValue; samplePos--) {
					sampleList[samplePos] = sampleList[samplePos - 1];
    }
    sampleList[samplePos] = sampleValue;
    sampleCnt++;
  };
  // Calculate median
  sampleValue = sampleList[samples >> 1];
  return sampleList[samples >> 1];;
}
