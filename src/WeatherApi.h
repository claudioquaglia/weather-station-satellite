#pragma once

#include <ESP8266HTTPClient.h>

typedef struct WeatherApiConditionData {
  // Weather condition text
  String text;

  // Weather icon url
  String icon;

  // Weather condition unique code.
  int code;
} WeatherConditionData;

typedef struct WeatherApiLocationData {
  // Location name
  String name;

  // Region or state of the location, if availa
  String region;

  // Location country
  String country;

  // Latitude in decimal degree
  float lat;

  // Longitude in decimal degree
  float lon;

  // Time zone name
  String tz_id;

  // Local date and time in unix time
  int localtime_epoch;

  // Local date and time
  String localtime;
} WeatherLocationData;

typedef struct WeatherApiCurrentData {
  // Local time when the real time data was updated.
  String last_updated;

  // Local time when the real time data was updated in unix time.
  int last_updated_epoch;

  // Temperature in celsius
  float temp_c;

  // Temperature in fahrenheit
  float temp_f;

  // Feels like temperature in celsius
  float feelslike_c;

  // Feels like temperature in fahrenheit
  float feelslike_f;

  // Weather condition struct
  WeatherApiConditionData condition;

  // Wind speed in miles per hour
  float wind_mph;

  // Wind speed in kilometer per hour
  float wind_kph;

  // Wind direction in degrees
  int wind_degree;

  // Wind direction as 16 point compass. e.g.: NSW
  String wind_dir;

  // Pressure in millibars
  float pressure_mb;

  // Pressure in inches
  float pressure_in;

  // Precipitation amount in millimeters
  float precip_mm;

  // Precipitation amount in inches
  float precip_in;

  // Humidity as percentage
  int humidity;

  // Cloud cover as percentage
  int cloud;

  // Whether to show day condition icon or night icon (1 = Yes 0 = No)
  int is_day;

  // UV index
  float uv;

  // Wind gust in miles per hour
  float gust_mph;

  // Wind gust in kilometer per hour
  float gust_kph;
} WeatherApiCurrentData;

typedef struct WeatherApiAstronomyData {
  // Sunrise local time
  String sunrise;

  // Sunset local time
  String sunset;

  // Moonrise local time
  String moonrise;

  // Moonset local time
  String moonset;

  // Moon phases
  String moon_phase;

  // Moon illumination
  int moon_illumination;
} WeatherApiAstronomyData;

typedef struct WeatherApiData {
  WeatherApiLocationData location;
  WeatherApiCurrentData current;
  WeatherApiAstronomyData astronomy;
} WeatherApiData;

class WeatherApi {
  private:
    const String host = "http://api.weatherapi.com";
    const int port = 80;
    WeatherApiData *data;
    boolean metric = true;
    String language;

    void doUpdate(WeatherApiData* data, String weatherPath, String astronomyPath);
    String currentDateTime();
    String startRequest(const String apiUrl, WiFiClient* wifiClient, HTTPClient* httpClient);
    String handleRequestError(int errorCode);
    String buildWeatherPath(String appId, String locationParameter);
    String buildAstronomyPath(String appId, String locationParameter);

  public:
    WeatherApi();
    void update(WeatherApiData *data, String appId, String location);

    void setMetric(boolean metric) {this->metric = metric;}
    boolean isMetric() { return metric; }
    void setLanguage(String language) { this->language = language; }
    String getLanguage() { return language; }
};
