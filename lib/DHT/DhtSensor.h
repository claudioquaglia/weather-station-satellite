#pragma once

#include <Arduino.h>
#include <DHT.h>

typedef struct DhtSensorData {
  float humidity;
  float temperature;
} DhtSensorData;

class DhtSensor {
  private:
    DHT sensor;
    uint8_t bits[5];
    DhtSensorData *data;
    boolean metric = true;

    float readTemperature();
    float readHumidity();

  public:
    DhtSensor(uint8_t sensorPin, uint8_t sensorType);
    String getType();
    void setMetric(boolean metric);
    boolean isMetric();
    void update(DhtSensorData *data);
};
