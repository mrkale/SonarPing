<a id="library"></a>
# SonarPing
Library for ultrasonic sensor HC-SR04.
- Library uses interrupts for measuring sound reflection times.
- Library uses only SI measurement units. The conversion to imperial units should be provided in a sketch code or in a separate library in order to avoid code duplicities in sketches using multiple libraries with the same conversion functionalities.
- Due to practical precision of the sensors the distance is always expressed in whole centimeters.
- Temperature compensation is limited to the range -128 to +127 centigrades, which contains usual working range of the sensors.
- Temperature compensation of sound speed can be used.
- Sound speed in meters per second is calculated from the statement
  ```soundSpeed (m/s) = 331.3 (m/s) + 0.6 * temperature (degC)```


<a id="credit"></a>
## Credit
The implementation has been inspired by the library [NewPing](http://code.google.com/p/arduino-new-ping/) from [Tim Eckel](teckel@leethost.com).


<a id="dependency"></a>
## Dependency
- **Arduino.h**: Main include file for the Arduino SDK version greater or equal to 100.
- **avr/io.h**
- **avr/interrupt.h**

<a id="constants"></a>
## Constants
- **SONARPING\_VERSION**: Semantic library version
- **SONARPING_DISTANCE_MIN**: Minimal measuring distance (cm)
- **SONARPING_DISTANCE_MAX**: Maximal measuring distance (cm)
- **SONARPING_TEMPERATURE_DEF**: Default ambient temperature (degC)
- **SONARPING_DELAY_INTERPING**: Minimal delay in milliseconds between pings from specification
- **SONARPING_DELAY_MAX**: Maximum microseconds for sensor to start the ping (SRF06 is the highest measured, just under 18 ms)

 
<a id="interface"></a>
## Interface
- [SonarPing()](#SonarPing)
- [getDistance()](#getDistance)
- [setTemperature()](#setTemperature)


<a id="SonarPing"></a>
## SonarPing()
#### Description
Overloaded constructor sets configuration parameters for library and measurement. They all are stored in the private variables of an library instance object.
- Maximal and minimal distance determines the expected range of measurement. Results outside this range are considered as erroneous ones and are ignored.
- Measurement range is limited by hardcoded limits 2 to 500 cm. Limits outside this range is defaulted to its bounderies.
- Overloaded constructor uses just one pin.

#### Syntax
    SonarPing(uint8_t trigger_pin, uint8_t echo_pin, uint16_t distance_max, uint16_t distance_min);
    SonarPing(uint8_t trigger_pin, uint16_t distance_max, uint16_t distance_min);

#### Parameters
<a id="trigger_pin"></a>
- **trigger_pin**: Digital pin number for sensor's trigger pin.


<a id="echo_pin"></a>
- **echo_pin**: Digital pin number for sensor's echo pin.
  - It is ignored in one-pin modus.


<a id="distance_max"></a>
- **distance_max**: Maximal accepted measured distance from the sensor to a reflector in centimeters.
  - *Valid values*: positive integer in the range [distance_min](#distance_min) to 500
  - *Default value*: 400


<a id="distance_min"></a>
- **distance_min**: Minimal accepted measured distance from the sensor to a reflector in centimeters.
  - *Valid values*: positive integer in the range 2 to 500
  - *Default value*: 2

#### Returns
Object perfoming the ultrasonic measurement.

#### Example

``` cpp
sonar = SonarPing(2, 3);          // Default measurement range
sonar = SonarPing(2, 3, 100);     // Measured limited distance, e.g., a water barrel
sonar = SonarPing(2, 3, 100, 50); // Measured limited range, e.g., a water level
```

[Back to interface](#interface)


<a id="getDistance"></a>
## getDistance()
#### Description
The method measures distance from the sensor to a reflector based on measuring sound wave time to it and back. Method corrects the measurement by temperature stored in instance object before measurement.

#### Syntax
    void getDistance();

#### Parameters
None

#### Returns
Distance in centimeters

#### See also
[SonarPing()](#SonarPing)
[setTemperature()](#setTemperature)

[Back to interface](#interface)


<a id="setTemperature"></a>
## setTemperature()
#### Description
The method stores the ambient air temperature to the instance object.
  - Default temperature is set to 20 degC at object initialization.
  - It is not needed to set temperature before every measurement, just then the temperature has changed significantly.

#### Syntax
    void setTemperature(int8_t temperature);

#### Parameters
<a id="temperature"></a>
- **temperature**: Ambient air temperature in centigrades.
  - *Valid values*: integer in the range -128 to +127
  - *Default value*: 20

#### Returns
None

#### See also
[SonarPing()](#SonarPing)
[getDistance()](#getDistance)

[Back to interface](#interface)
