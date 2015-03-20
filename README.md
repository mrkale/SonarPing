# SonarPing
The Arduino library for ultrasonic sensor HC-SR04.
- The library may be used for one-pin sensors as well.

- Library is inspired by the Tim Eckel's library NewPing version 1.5
  and 1.6 beta.

- Functionalities:

  - Temperature compensation to sound speed.
  - Digital filtering with interquartile mean, median, and moving mean.
  - Expected measurement range (min. ~ max.).
  - Only SI measurement units.
    The conversion to imperial units should be provided in a sketch code
    or in a separate library in order to avoid code duplicities in sketches
    using multiple libraries with the same conversion functionalities.
    

- Due to practical precision of the sensors the distances are allways 
  expressed in whole centimeters.

- Temperature compensation is limited to the range -128°C to +127°C, which
  contains usual working range of the sensors.
- Sound speed in meters per second with temperature correction is calculated
  from the statement
  `soundSpeed = 331.3 + 0.6 * centigrades`
