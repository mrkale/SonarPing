/*
  NAME:
  SonarPing

  DESCRIPTION:
  Library for ultrasonic sensor HC-SR04.
  - Library is inspired by the Tim Eckel's library NewPing version 1.5
    and 1.6 beta.
  - Library uses interrupts for measuring sound reflection times.
  - Library uses only SI measurement units. The conversion to imperial units
    should be provided in a sketch code or in a separate library in order
    to avoid code duplicities in sketches using multiple libraries with
    the same conversion functionalities.
  - Due to practical precision of the sensors the distance is always
    expressed in whole centimeters.
  - Temperature compensation is limited to the range -128 to +127 centigrades,
    which contains usual working range of the sensors.
  - Temperature compensation of sound speed can be used. It is limited
    to the range -128 to +127 centigrades, which contains usual working
    range of the sensors.
  - Sound speed in meters per second is calculated from the statement
    soundSpeed (m/s) = 331.3 (m/s) + 0.606 * temperature (degC)

  LICENSE:
  This program is free software; you can redistribute it and/or modify
  it under the terms of the license GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
  (related to original code) and MIT License (MIT) for added code.

  CREDENTIALS:
  Author: Libor Gabaj
  GitHub: https://github.com/mrkaleArduinoLib/SonarPing.git

  CREDITS:
  Tim Eckel - teckel@leethost.com
  http://code.google.com/p/arduino-new-ping/
 */
#ifndef SONARPING_H
#define SONARPING_H
#define SONARPING_VERSION "SonarPing 1.2.0"

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
  #include <pins_arduino.h>
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// You shoudln't change these values unless you really know what you're doing.
#define SONARPING_DISTANCE_MIN    2      // Minimal measuring distance (cm)
#define SONARPING_DISTANCE_MAX    500    // Maximal measuring distance (cm)
#define SONARPING_TEMPERATURE_DEF 20     // Default ambient temperature (degC)
#define SONARPING_DELAY_INTERPING 29     // Minimal delay in milliseconds between pings from specification
#define SONARPING_DELAY_MAX       18000  // Maximum microseconds for sensor to start the ping (SRF06 is the highest measured, just under 18 ms)

class SonarPing {
public:
//-------------------------------------------------------------------------
// Public methods
//-------------------------------------------------------------------------

/*
  Constructor with configuration parameters

  DESCRIPTION:
  Setting up configuration parameters for library and measurement. They all
  are stored in the private variables.
  - Maximal and minimal distance determines the expected range of measurement.
    Results outside this range are considered as erroneous ones and are
    ignored.
  - Measurement range is limited by hardcoded limits 2 to 500 cm. Limits
    outside this range is defaulted to its bounderies.
  - Overloaded constructor uses just one pin.

  PARAMETERS:
  trigger_pin  - digital pin number for sensor's trigger pin.
  echo_pin     - digital pin number for sensor's echo pin.
                 It is ignored in one-pin modus.
  distance_max - maximal accepted measured distance from the sensor to
                 a reflector in centimeters.
                 - Defaulted to 400 cm.
                 - Limited to 500 cm.
  distance_min - minimal accepted measured distance from the sensor to
                 a reflector in centimeters.
                 - Defaulted to 2 cm.
                 - Limited to 0 cm.

  RETURN: object
*/
  SonarPing(uint8_t trigger_pin, uint8_t echo_pin, \
            uint16_t distance_max = SONARPING_DISTANCE_MAX, \
            uint16_t distance_min = SONARPING_DISTANCE_MIN);
/*
  Overloaded constructor for one-pin sensors.
*/
  SonarPing(uint8_t trigger_pin, \
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

  RETURN: Distance in centimeters
*/
  uint16_t getDistance();

  /*
  Set ambient air temperature in centigrades for sound speed correction.

  DESCRIPTION:
  The method stores the ambient temperature to the class instance object.
  - Default temperature is set to 20 degC at object initialization.
  - It is not needed to set temperature before every measurement, just then
    the temperature has changed significantly.

  PARAMETERS:
  temperature - ambient air temperature in centigrades
                - Limited to -128 ~ +127 degC by data type.
                - Defaulted to 20 degC.

  RETURN: none
*/
  void setTemperature(int8_t temperature = SONARPING_TEMPERATURE_DEF);

private:
//-------------------------------------------------------------------------
// Private attributes
//-------------------------------------------------------------------------
  uint8_t _triggerBit;
  uint8_t _echoBit;
  volatile uint8_t *_triggerOutput;
  volatile uint8_t *_triggerMode;
  volatile uint8_t *_echoInput;
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
  - In the temperature range -15 ~ +40 degC it is in the range 33 ~ 28 us/cm
    and temperature influence is 2 ~ -3 us/cm (2.8 ~ -6.8%).

  PARAMETERS: none

  RETURN: Sound pace in microseconds per centimeter
*/
  uint8_t soundPace(void);

  /*
  Measure sound ping round trip time to reflector and back in microseconds.

  DESCRIPTION:
  The method makes the sensor to broadcast a sound impuls, then waits for
  the echo pulse and measures its length as the time of sound round trip
  from sensor to a reflector and back.
  - Method cancels if broadcasting takes longer than expected by sensor
    specification.
  - Method cancels measuring the echo pulse if it takes longer or shorter
    than expected from maximal and minimal distance range set in constructor.
  - Method corrects sound speed by temperature stored to class object
    before measurement.

  PARAMETERS: none

  RETURN: Sound round trip time in microseconds
*/
  uint16_t ping_us();
  uint16_t distance2time(uint16_t distance);
};
#endif
