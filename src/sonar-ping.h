/*
  NAME:
  SonarPing

  DESCRIPTION:
  Library for ultrasonic sensor HC-SR04 or similar sensors.
  - Library uses function pulseIn() for measuring sound reflection times.
  - Library intentionally uses only SI measurement units. The conversion
    to imperial units should be provided in a sketch code or in a separate
    library in order to avoid duplicities in code using multiple
    libraries with the same conversion functionality.
  - Library does not intentionally use statistical processing of measured
    values (ping time, distance). There are separate libraries for that
    purpose to use in order to avoid duplicities in code.
  - Library uses the temperature compensation for air speed. It is limited
    to the temperature range from -128 to +127 centigrades (degrees
    of Celsius), which contains usual working range of an intended sensor.
  - Sound speed is calculated in meters per second from the statement
    soundSpeed (m/s) = 331.3 (m/s) + 0.606 * temperature (degC). Default
    temperature is set to 20 degC.
  - Library measures the distance in whole centimeters in regards to
    practical accuracy of ultrasonic sensors.

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
  (related to original code) and MIT License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkalePhotonArduinoLib/SonarPing.git
 */
#ifndef SONARPING_H
#define SONARPING_H
#define SONARPING_VERSION "SonarPing 1.0.0"

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
  #include <inttypes.h>
  #include <Wire.h>
#elif defined(PARTICLE)
  #include <Particle.h>
#endif

// You shoudln't change these values unless you really know what you're doing.
#define SONARPING_NAN             0      // Represents undefined ping time or distance
#define SONARPING_DISTANCE_MIN    2      // Minimal measuring distance (cm)
#define SONARPING_DISTANCE_MAX    500    // Maximal measuring distance (cm)
#define SONARPING_TEMPERATURE_DEF 20     // Default ambient temperature (degC)
#define SONARPING_DELAY_INTERPING 29     // Minimal delay in milliseconds between pings from specification


#if defined(ARDUINO)
#define digitalWriteFast digitalWrite
#endif

class SonarPing
{
public:
//-------------------------------------------------------------------------
// Public methods
//-------------------------------------------------------------------------

/*
  Constructor.

  DESCRIPTION:
  Setting up configuration parameters for library and measurement. They all
  are stored in the private variables.
  - Maximal and minimal distance determines the expected range of measurement.
    Results outside this range are considered as erroneous ones and are
    ignored.
  - Measurement range is limited by limits 2 to 500 cm due to hardware
    limitation. Limits outside this range are defaulted to those bounderies.

  PARAMETERS:
  trigger_pin   - Digital pin number for sensor's trigger pin.
                  - Data type: non-negative integer
                  - Default value: none
                  - Limited range: 0 ~ 127 (determined by hardware)
  echo_pin      - Digital pin number for sensor's echo pin.
                  - Data type: non-negative integer
                  - Default value: none
                  - Limited range: 0 ~ 127 (determined by hardware)
  distance_max  - Maximal accepted measured distance from the sensor to
                  a reflector in centimeters.
                  - Data type: non-negative integer
                  - Default value: SONARPING_DISTANCE_MAX
                  - Limited range: SONARPING_DISTANCE_MIN ~ SONARPING_DISTANCE_MAX
  distance_min  - Minimal accepted measured distance from the sensor to
                  a reflector in centimeters.
                  - Data type: non-negative integer
                  - Default value: SONARPING_DISTANCE_MIN
                  - Limited range: SONARPING_DISTANCE_MIN ~ SONARPING_DISTANCE_MAX

  RETURN: object
*/
  SonarPing(uint8_t trigger_pin, uint8_t echo_pin, \
            uint16_t distance_max = SONARPING_DISTANCE_MAX, \
            uint16_t distance_min = SONARPING_DISTANCE_MIN);


 /*
  Measure distance in centimeters from the sensor to a reflector.

  DESCRIPTION:
  The method measures distance from the sensor to a reflector based
  on measuring sound wave time to it and back.
  - Method corrects the measurement by temperature stored in class object
    before measurement.

  PARAMETERS: none

  RETURN:
  Distance in centimeters.
*/
  uint16_t measureDistance();


//-------------------------------------------------------------------------
// Public setters
//-------------------------------------------------------------------------

/*
  Setter for ambient air temperature in centigrades.

  DESCRIPTION:
  The method stores the ambient temperature to the class instance object for
  sound speed correction.
  - Default temperature is set to 20 degC in constructor.
  - It is not needed to set temperature before every measurement, just then
    the temperature has changed significantly.

  PARAMETERS:
  temperature - Ambient air temperature in centigrades.
                - Data type: integer
                - Default value: SONARPING_TEMPERATURE_DEF
                - Limited range: -128 ~ +127

  RETURN: none
*/
  void setTemperature(int8_t temperature = SONARPING_TEMPERATURE_DEF);


//-------------------------------------------------------------------------
// Public getters
//-------------------------------------------------------------------------
  int8_t    getTemperature();
  uint16_t  getDistanceMax();
  uint16_t  getDistanceMin();


private:
//-------------------------------------------------------------------------
// Private attributes
//-------------------------------------------------------------------------
  uint8_t  _pinTrig;
  uint8_t  _pinEcho;
  uint16_t _maxDistance;
  uint16_t _minDistance;
  int8_t   _temperature = SONARPING_TEMPERATURE_DEF;


//-------------------------------------------------------------------------
// Private methods
//-------------------------------------------------------------------------
/*
  Calculate sound pace in microseconds per centimeter with temperature correction.

  DESCRIPTION:
  The method calculates number of microseconds the sound wave needs to travel
  one centimeter at the current temperature set previously.
  - It is reciprocal value to sound speed.
  - For a reasonable precision it is expressed as integer.
  - Within the temperature range -15 ~ +40 degC it is in the range 31 ~ 28 us/cm
    and temperature influence is +2 ~ -1 us/cm (+6.6 ~ -3.4%) in comparison
    to reference temperature 20 degC.

  PARAMETERS: none

  RETURN:
  Sound pace in microseconds per centimeter.
*/
  uint8_t soundPace(void);


/*
  Measure sound ping round trip time to reflector and back in microseconds.

  DESCRIPTION:
  The method makes the sensor to broadcast a sound impulse, then waits for
  the echo pulse and measures its length as the time of sound round trip
  from sensor to a reflector and back.
  - Method uses firmware function pulseIn() so that the default timeout is
    3 seconds on Particle platform and 1 second on Arduino platform at waiting
    for echo signal.
  - Method cancels measuring the echo pulse if it takes longer or shorter
    than expected from maximal and minimal distance range set in constructor.
  - Method corrects sound speed by temperature stored in class object before
    measurement.

  PARAMETERS: none

  RETURN:
  Sound round trip time in microseconds.
*/
  uint16_t pingTime();

};

#endif
