#include <Arduino.h>
#include <ESP8266WiFi.h>          // ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <WeatherApi.h>
#include "settings.h"

WeatherApiData currentWeather;

void updateData();

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  Serial.begin(115200);

  // WiFiManager
  // Local initialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  // reset saved settings
  //wifiManager.resetSettings();

  // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    // ESP.reset();
    delay(1000);
  }

  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi with hostname: " + hostname);

  updateData();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void updateData() {
  time_t now;

  // drawProgress(10, "Updating time...");
  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  while((now = time(nullptr)) < NTP_MIN_VALID_EPOCH) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.printf("Current time: %d\n", (int)now);

  // drawProgress(50, "Updating conditions...");
  Serial.println("Updating data");
  WeatherApi *currentWeatherClient = new WeatherApi();
  currentWeatherClient->setMetric(IS_METRIC);
  currentWeatherClient->setLanguage(WEATHER_API_LANGUAGE);
  currentWeatherClient->update(&currentWeather, WEATHER_API_APP_ID, WEATHER_API_LOCATION);
  delete currentWeatherClient;
  currentWeatherClient = nullptr;

  delay(1000);
}
