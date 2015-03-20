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
#ifndef SonarPing_h
#define SonarPing_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
	#include <pins_arduino.h>
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

// Probably shoudln't change these values unless you really know what you're doing.
#define LIMIT_MAX_DISTANCE 500    // Maximal measuring distance
#define LIMIT_MIN_DISTANCE 2      // Minimal measuring distance
#define DEFAULT_TEMPERATURE 20    // Default ambient temperature for sound speed correction
#define MAX_SENSOR_DELAY 18000    // Maximum micros for sensor to start the ping (SRF06 is the highest measured, just under 18 ms)
#define INTERPING_DELAY 29        // Minimal delay in milliseconds between pings from specification
#define DEFAULT_SAMPLES_IQMEAN 8  // Number of samples for intequartile mean calculation
#define DEFAULT_SAMPLES_MEDIAN 5  // Number of samples for median calculation

class SonarPing {
public:
/*
  Constructor with configuration parameters
  
  DESCRIPTION:
  Setting up configuration parameters for library and measurement. They all
  are stored in the private variables.
  * Maximal and minimal distance determines the expected range of measurement.
    Results outside this range are considered as erroneous ones and are
    ignored.
  * Measurement range is limited by hardcoded limits 2 to 500 cm. Limits
    outside this range is defaulted to its bounderies.
  * Overloaded constructor uses just one pin.
  
  PARAMETERS:
  trigger_pin     - digital pin number for sensor's trigger pin.
  echo_pin        - digital pin number for sensor's echo pin.
                    It is ignored in one-pin modus.
  max_cm_distance - maximal accepted measured distance from the sensor to
                    a reflector in centimeters.
                    Defaulted to 400 cm.
                    Limited to 500 cm.
  min_cm_distance - minimal accepted measured distance from the sensor to
                    a reflector in centimeters.
                    Defaulted to 2 cm.
                    Limited to 0 cm.
  
  RETURN:	none
*/
  SonarPing(uint8_t trigger_pin, uint8_t echo_pin, \
          uint16_t max_cm_distance = LIMIT_MAX_DISTANCE, \
          uint16_t min_cm_distance = LIMIT_MIN_DISTANCE);
/*
  Overloaded constructor for one-pin sensors.
*/
  SonarPing(uint8_t trigger_pin, \
          uint16_t max_cm_distance = LIMIT_MAX_DISTANCE, \
          uint16_t min_cm_distance = LIMIT_MIN_DISTANCE);
/*
  Measure distance in centimeters from the sensor to a reflector.
  
  DESCRIPTION:
  The method measures distance from the sensor to a reflector based
  on measuring sound wave time to it and back.
  * Method corrects the measurement by temperature stored to class object 
    before measurement.
  
  PARAMETERS:
  none
  
  RETURN:	Distance in centimeters
*/
  uint16_t distance();
/*
  Measure distance in centimeters statistically corrected by intequartile mean.
  
  DESCRIPTION:
  The method performes a serie of measurements and calculates the arithmetic
  mean from serie's second and third quarter of measuring values.
  * Method aligns the input number of samples in a serie to nearest multiple
    of 4.
  * After distance measuring of all samples in a serie and sorting them
    by value ascending the method gets rid of the first and last quarter.
    Finally the method calculates the arithmetic mean from samples in remaining
    inner half of a serie.
  * The method applies temperature correction and ignores measurements outside
    expected range set in the constructor at creating a serie of samples.
      
  PARAMETERS:
  samples   - number of samples in a serie for statistical correction
              Aligned to nearest multiple of 4.
              Defaulted to 8.
  
  RETURN:	Interquartile distance in centimeters
*/
  uint16_t distance_iqmean(uint8_t samples = DEFAULT_SAMPLES_IQMEAN);
/*
  Measure distance in centimeters statistically corrected by median.
  
  DESCRIPTION:
  The method performes a serie of measurements and calculates the median
  from measured values.
  * Method aligns the input number of samples in a serie to nearest odd integer.
  * After distance measuring of all samples in a serie and sorting them
    by value ascending the method returns the middle sample, which is median.
  * The method applies temperature correction and ignores measurements outside
    expected range set in the constructor at creating a serie of samples.
      
  PARAMETERS:
  samples   - number of samples in a serie for statistical correction
              Aligned to nearest odd number.
              Defaulted to 5.
  
  RETURN:	Interquartile distance in centimeters
*/
  uint16_t distance_median(uint8_t samples = DEFAULT_SAMPLES_MEDIAN);
/*
  Calculate moving mean.
  
  DESCRIPTION:
  The method calculates moving mean from the input value and the recently
  calculated moving mean value stored in the method.
  * At the very first usage of the method the current value is considered
    as a moving mean.
  * Current moving mean is calculated as a arithmetic mean of the recent
    moving mean and the current value.
  * The moving mean is stored in the static variable of the method.
  * Moving mean is suitable for smoothing measurements of slow alternated
    and continuos events, e.g., at observing water level changes.    
  * The method is not appropriate for rapid changing events like observing
    moving objects.  
      
  PARAMETERS:
  currentValue - value for calculating moving mean
  
  RETURN:	Moving mean of input value
*/
  uint16_t calc_mvmean(uint16_t currentValue);
/*
  Set ambient air temperature in centigrades for sound speed correction.
  
  DESCRIPTION:
  The method stores the ambient temperature to the class instance object.
  * Default temperature is set to 20 centigrades at object initialization.
  * It is not needed to set temperature before every measurement, just then
    the temperature has changed significantly. 
  
  PARAMETERS:
  temperature     - ambient temperature in centigrades
                    Limited to -128°C ~ +127°C by data type.
                    Defaulted to 20°C.
  
  RETURN:	none
*/
  void setTemperature(int8_t temperature = DEFAULT_TEMPERATURE);

private:
  uint8_t _triggerBit;
  uint8_t _echoBit;
  volatile uint8_t *_triggerOutput;
  volatile uint8_t *_triggerMode;
  volatile uint8_t *_echoInput;
  uint16_t _maxDistance;
  uint16_t _minDistance;
  int8_t   _temperature = DEFAULT_TEMPERATURE;
/*
  Calculate sound pace in microseconds per centimeter with temperature correction.
  
  DESCRIPTION:
  The method calculates number of microseconds the sound wave needs to travel
  one centimeter at the current temperature set previously.
  * It is reciprocal value to sound speed.
  * For a reasonable precision it is expressed as integer.
  * In the temperature range -15°C ~ +40°C it is in the range 33 ~ 28 us/cm
  * and temperature influence is 2 ~ -3 us/cm (2.8 ~ -6.8%).
  
  PARAMETERS:
  none
  
  RETURN:	Sound pace in microseconds per centimeter
*/
  uint8_t  soundPace(void);
/*
  Measure sound ping round trip time to reflector and back in microseconds.
  
  DESCRIPTION:
  The method makes the sensor to broadcast a sound impuls, then waits for
  the echo pulse and measures its length as the time of sound round trip
  from sensor to a reflector and back.
  * Method cancels if broadcasting takes longer than expected by sensor
    specification.
  * Method cancels measuring the echo pulse if it takes longer or shorter
    than expected from maximal and minimal distance range set in constructor.
  * Method corrects sound speed by temperature stored to class object 
    before measurement.
  
  PARAMETERS:
  none
  
  RETURN:	Sound round trip time in microseconds
*/
  uint16_t ping_us();
  // Internal calculations
  uint16_t distance2time(uint16_t distance);
  uint16_t calc_iqmean(uint16_t (SonarPing::*userFunc)(void), uint8_t samples);
  uint16_t calc_median(uint16_t (SonarPing::*userFunc)(void), uint8_t samples);
};
#endif
