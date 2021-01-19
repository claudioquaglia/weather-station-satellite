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
  return this->sensor.readTemperature(this->metric);
}

float DhtSensor::readHumidity() {
  return this->sensor.readHumidity();
}

void DhtSensor::update(DhtSensorData *data) {
  this->data = data;

  Serial.print("Sensor Class\n");
  Serial.println(this->sensor.read());
  Serial.print(this->readHumidity());
  Serial.print(" - ");
  Serial.print(this->readTemperature());
  Serial.print("\n");

  this->data->humidity = this->readHumidity();
  this->data->temperature = this->readTemperature();
}
