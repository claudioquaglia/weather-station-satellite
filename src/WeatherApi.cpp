#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "WeatherApi.h"

WeatherApi::WeatherApi() {}

void WeatherApi::update(WeatherApiData *data, String appId, String location) {
  doUpdate(data, buildWeatherPath(appId, location), buildAstronomyPath(appId, location));
}

// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime for more information about date/time format
String WeatherApi::currentDateTime() {
  time_t now = time(nullptr);
  struct tm tstruct;
  char buf[80];
  tstruct = *gmtime(&now);
  strftime(buf, sizeof(buf), "%F", &tstruct);

  return buf;
};

String WeatherApi::buildWeatherPath(String appId, String locationParameter) {
  String units = metric ? "metric" : "imperial";
  return "/v1/current.json?key=" + appId + "&q=" + locationParameter + "&lang=" + language;
}

String WeatherApi::buildAstronomyPath(String appId, String locationParameter) {
  String today = this->currentDateTime();
  return "/v1/astronomy.json?key=" + appId + "&q=" + locationParameter + "&dt=" + today + "&lang=" + language;
}


String WeatherApi::startRequest(String apiUrl, WiFiClient* wifiClient, HTTPClient* httpClient) {
  Serial.printf("[HTTP] Calling api: %s\r\n", apiUrl.c_str());
  String response;

  httpClient->begin(*wifiClient, apiUrl);
  int httpCode = httpClient->GET();

  Serial.println(httpCode);

  if (httpCode > 0) {
    if (httpCode != HTTP_CODE_OK || httpCode != HTTP_CODE_MOVED_PERMANENTLY) {
      Serial.printf("[HTTP] GET... unexpected status code, error: %s\n", httpClient->errorToString(httpCode).c_str());
      response = "";
    }

    response = httpClient->getString();
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", httpClient->errorToString(httpCode).c_str());
    response = "";
  }

  httpClient->end();

  return response;
}

void WeatherApi::doUpdate(WeatherApiData* data, String weatherPath, String astronomyPath) {
  WiFiClient client;
  HTTPClient http;

  char weatherApiURL[1024];
  char astronomyApiURL[1024];

  sprintf(weatherApiURL, "%s:%u%s", host.c_str(), port, weatherPath.c_str());
  sprintf(astronomyApiURL, "%s:%u%s", host.c_str(), port, astronomyPath.c_str());

  String weatherJson = this->startRequest(weatherApiURL, &client, &http);
  String astronomyJson = this->startRequest(astronomyApiURL, &client, &http);
}
