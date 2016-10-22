#include "SonarPing.h"

SonarPing::SonarPing(uint8_t trigger_pin, uint8_t echo_pin, \
  uint16_t distance_max, uint16_t distance_min)
{
  // Bitmask for pins
	_triggerBit = digitalPinToBitMask(trigger_pin);
	_echoBit = digitalPinToBitMask(echo_pin);
  // Data Direction Registers (DDR - Input/Output)
	_triggerOutput = portOutputRegister(digitalPinToPort(trigger_pin));
	_echoInput = portInputRegister(digitalPinToPort(echo_pin));
  // Port Mode Register for the trigger pin (PORT - HIGH/LOW)
	_triggerMode = (uint8_t *) portModeRegister(digitalPinToPort(trigger_pin));
  // Maintain input parameters
  _minDistance = constrain(distance_min, SONARPING_DISTANCE_MIN, SONARPING_DISTANCE_MAX);
  _maxDistance = constrain(distance_max, _minDistance, SONARPING_DISTANCE_MAX);
}
SonarPing::SonarPing(uint8_t trigger_pin, uint16_t distance_max, uint16_t distance_min)
{
  SonarPing(trigger_pin, trigger_pin, distance_max, distance_min);
}

// Measure sound ping round trip time (microseconds) to reflector
uint16_t SonarPing::ping_us()
{
  uint32_t deadline;
  uint16_t pingTime = distance2time(_maxDistance);
  // Wait necessary time from potential previous ping (from specs)
  delay(SONARPING_DELAY_INTERPING);
  // Broadcast sound ping
	*_triggerMode |= _triggerBit;    // Set trigger pin to OUTPUT
	*_triggerOutput &= ~_triggerBit; // Set the trigger pin to LOW
  delayMicroseconds(4);            // Wait for pin to go LOW (from testing)
	*_triggerOutput |= _triggerBit;  // Set trigger pin to go HIGH - send ping
	delayMicroseconds(10);           // Wait for trigger pin become HIGH - from specs
	*_triggerOutput &= ~_triggerBit; // Set trigger pin to go LOW
  *_triggerMode &= ~_triggerBit;   // Set trigger pin to INPUT for one pin mode
  // Wait for start of measuring echo pulse
	deadline =  micros() + SONARPING_DELAY_MAX;  // Deadline for pulse start
	while (*_echoInput & _echoBit && micros() <= deadline){} // Wait for pin to go LOW
	while (!(*_echoInput & _echoBit)) {      // Wait for pin to go HIGH - start pulse
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
uint16_t SonarPing::getDistance()
{
  return ping_us() / soundPace() / 2 + 0.5;
}

// Store current temperature for further calculations
void SonarPing::setTemperature(int8_t temperature)
{
  _temperature = temperature;
}

// Sound pace corrected by temperature
uint8_t SonarPing::soundPace(void)
{
  return (1000000.0 / (33130.0 + 60.6 * _temperature));
}

// Sound travel time
uint16_t SonarPing::distance2time(uint16_t distance)
{
  return distance * 2 * soundPace();
}
