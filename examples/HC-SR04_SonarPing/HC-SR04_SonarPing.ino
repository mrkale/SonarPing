#include <SonarPing.h>

const byte pinTrigger = 2;
const byte pinEcho = 3;

SonarPing sonar(pinTrigger, pinEcho);

unsigned int distance;

void setup()
{
  Serial.begin(9600);
  // Set ambient temperature (should be located in loop for real measurement)
  sonar.setTemperature(25);
  // Header
  Serial.println("Distance\tMedian\tIQmean\tMovingMean");
}

void loop()
{
  // Direct measurement
  distance = sonar.distance();
  Serial.print(distance);
  Serial.print("\t\t");
  // Median
  distance = sonar.distance_median();
  Serial.print(distance);
  Serial.print("\t");
  // Interquartile mean
  distance = sonar.distance_iqmean();
  Serial.print(distance);
  Serial.print("\t");
  // Moving mean from interquartile mean
  distance = sonar.calc_mvmean(distance);
  Serial.println(distance);
  delay(1000);
}
