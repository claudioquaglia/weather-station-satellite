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

  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode != HTTP_CODE_OK) {
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

void WeatherApi::mapLocationData(DynamicJsonDocument locationData) {
  this->data->location.name = locationData["name"].as<String>();
  this->data->location.region = locationData["region"].as<String>();
  this->data->location.country = locationData["country"].as<String>();
  this->data->location.lat = locationData["lat"].as<float>();
  this->data->location.lon = locationData["lon"].as<float>();
  this->data->location.tz_id = locationData["tz_id"].as<String>();
  this->data->location.localtime_epoch = locationData["localtime_epoch"].as<int>();
  this->data->location.localtime = locationData["localtime"].as<String>();
}

void WeatherApi::mapWeatherData(DynamicJsonDocument weatherData) {
  this->data->current.last_updated_epoch = weatherData["last_updated_epoch"].as<int>();
  this->data->current.last_updated = weatherData["last_updated"].as<String>();
  this->data->current.temp_c = weatherData["temp_c"].as<float>();
  this->data->current.temp_f = weatherData["temp_f"].as<float>();
  this->data->current.is_day = weatherData["is_day"].as<int>();
  this->data->current.condition.text = weatherData["condition"]["text"].as<String>();
  this->data->current.condition.icon = weatherData["condition"]["icon"].as<String>();
  this->data->current.condition.code = weatherData["condition"]["code"].as<int>();
  this->data->current.wind_mph = weatherData["wind_mph"].as<float>();
  this->data->current.wind_kph = weatherData["wind_kph"].as<float>();
  this->data->current.wind_degree = weatherData["wind_degree"].as<int>();
  this->data->current.wind_dir = weatherData["wind_dir"].as<String>();
  this->data->current.pressure_mb = weatherData["pressure_mb"].as<float>();
  this->data->current.pressure_in = weatherData["pressure_in"].as<float>();
  this->data->current.precip_mm = weatherData["precip_mm"].as<float>();
  this->data->current.precip_in = weatherData["precip_in"].as<float>();
  this->data->current.humidity = weatherData["humidity"].as<int>();
  this->data->current.cloud = weatherData["cloud"].as<int>();
  this->data->current.feelslike_c = weatherData["feelslike_c"].as<float>();
  this->data->current.feelslike_f = weatherData["feelslike_f"].as<float>();
  this->data->current.uv = weatherData["uv"].as<float>();
  this->data->current.gust_mph = weatherData["gust_mph"].as<float>();
  this->data->current.gust_kph = weatherData["gust_kph"].as<float>();
}

void WeatherApi::mapAstronomyData(DynamicJsonDocument astronomyData) {
  this->data->astronomy.sunrise = astronomyData["sunrise"].as<String>();
  this->data->astronomy.sunset = astronomyData["sunset"].as<String>();
  this->data->astronomy.moonrise = astronomyData["moonrise"].as<String>();
  this->data->astronomy.moonset = astronomyData["moonset"].as<String>();
  this->data->astronomy.moon_phase = astronomyData["moon_phase"].as<String>();
  this->data->astronomy.moon_illumination = astronomyData["moon_illumination"].as<int>();
}

void WeatherApi::parseData(String weatherJson, String astronomyJson) {
  // Use arduinojson.org/v6/assistant to compute the capacity.
  DynamicJsonDocument weatherDoc(1536);
  DynamicJsonDocument astronomyDoc(768);

  // Deserialize the JSON document
  DeserializationError weatherError = deserializeJson(weatherDoc, weatherJson);
  DeserializationError astronomyError = deserializeJson(astronomyDoc, astronomyJson);

  if (weatherError) {
    Serial.print(F("deserializeJson(weatherJson) failed: "));
    Serial.println(weatherError.f_str());
    return;
  }

  if (astronomyError) {
    Serial.print(F("deserializeJson(astronomyJson) failed: "));
    Serial.println(astronomyError.f_str());
    return;
  }

  this->mapLocationData(weatherDoc["location"]);
  this->mapWeatherData(weatherDoc["current"]);
  this->mapAstronomyData(astronomyDoc["astronomy"]["astro"]);
}

void WeatherApi::doUpdate(WeatherApiData* data, String weatherPath, String astronomyPath) {
  WiFiClient client;
  HTTPClient http;

  this->data = data;

  char weatherApiURL[1024];
  char astronomyApiURL[1024];

  sprintf(weatherApiURL, "%s:%u%s", host.c_str(), port, weatherPath.c_str());
  sprintf(astronomyApiURL, "%s:%u%s", host.c_str(), port, astronomyPath.c_str());

  String weatherJson = this->startRequest(weatherApiURL, &client, &http);
  String astronomyJson = this->startRequest(astronomyApiURL, &client, &http);

  this->parseData(weatherJson, astronomyJson);
}

