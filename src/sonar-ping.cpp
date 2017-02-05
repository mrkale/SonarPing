#include <sonar-ping.h>

SonarPing::SonarPing(uint8_t trigger_pin, uint8_t echo_pin, \
  uint16_t distance_max, uint16_t distance_min)
{
  // Sanitize arguments
  _minDistance = constrain(distance_min, SONARPING_DISTANCE_MIN, SONARPING_DISTANCE_MAX);
  _maxDistance = constrain(distance_max, _minDistance, SONARPING_DISTANCE_MAX);
  if (_minDistance > _maxDistance)
  {
    uint16_t t = _minDistance;
    _minDistance = _maxDistance;
    _maxDistance = t;
  }
  // Setup pins
  _pinTrig = trigger_pin;
  _pinEcho = echo_pin;
  pinMode(_pinTrig, OUTPUT);
  pinMode(_pinEcho, INPUT);
}


// Measure distance
uint16_t SonarPing::measureDistance()
{
  uint16_t ping, distance;
  uint8_t  pace;
  ping = pingTime();
  pace = soundPace() * 2; // Forward and back
  // Round up distance to whole centimeters
  distance = (ping + pace - 1) / pace;
  // Check distance to limits
  if (distance < _minDistance) return SONARPING_NAN;
  if (distance > _maxDistance) return SONARPING_NAN;
  return distance;
}

//-------------------------------------------------------------------------
// Setters
//-------------------------------------------------------------------------

// Store current ambient air temperature
void SonarPing::setTemperature(int8_t temperature)
{
  _temperature = temperature;
}

//-------------------------------------------------------------------------
// Getters
//-------------------------------------------------------------------------
int8_t   SonarPing::getTemperature() { return _temperature; };
uint16_t SonarPing::getDistanceMin() { return _minDistance; };
uint16_t SonarPing::getDistanceMax() { return _maxDistance; };


//-------------------------------------------------------------------------
// Private methods
//-------------------------------------------------------------------------

// Measure sound ping round trip time (microseconds) to reflector
uint16_t SonarPing::pingTime()
{
  uint16_t pingTime;
  // Wait necessary time from potential previous ping (from specs)
  delay(SONARPING_DELAY_INTERPING);
  // Broadcast sound ping
  digitalWriteFast(_pinTrig, LOW);
  delayMicroseconds(4);   // Wait for pin to go LOW (from testing)
  digitalWriteFast(_pinTrig, HIGH);
  delayMicroseconds(10);  // Wait for trigger pin become HIGH - from specs
  digitalWriteFast(_pinTrig, LOW);
  // Measure duration of echo signal
  pingTime = pulseIn(_pinEcho, HIGH);
  return pingTime;
}


// Sound pace corrected by temperature
uint8_t SonarPing::soundPace(void)
{
  return (uint8_t)(1000000.0 / (33130.0 + 60.6 * (float)_temperature));
}
