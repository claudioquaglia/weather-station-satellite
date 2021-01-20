#include "DHT.h"

#include "DhtSensor.h"

DhtSensor::DhtSensor(uint8_t sensorPin, uint8_t sensorType): sensor(sensorPin, sensorType) {
  this->sensor.begin();
}

void DhtSensor::setMetric(boolean metric) {
  this->metric = metric;
}

boolean DhtSensor::isMetric() {
  return this->metric;
}

float DhtSensor::readTemperature() {
  float prevTemperature = this->data->temperature;
  // the metric will be reversed 'cause the library ask for isFahrenheit
  float newTemperature = this->sensor.readTemperature(!this->metric);

  return !isnan(newTemperature) ? newTemperature : prevTemperature;
}

float DhtSensor::readHumidity() {
  float prevHumidity = this->data->humidity;
  float newHumidity = this->sensor.readHumidity();

  return !isnan(newHumidity) ? newHumidity : prevHumidity;
}

void DhtSensor::update(DhtSensorData *data) {
  this->data = data;

  this->data->humidity = this->readHumidity() - 10;
  this->data->temperature = this->readTemperature() - 1.5;
}
