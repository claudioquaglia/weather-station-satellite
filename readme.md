# Setup

- create `config.h` file into `/src` folder

```cpp
// The name of the host when it goes in pair mode
#define HOSTNAME "ESP8266-"

// The APP_ID from https://www.weatherapi.com/my/
#define WEATHER_API_APP_ID "APP_ID_HERE"

// The location you want get
#define WEATHER_API_LOCATION "Rome"

// The language of the API response
#define WEATHER_API_LANGUAGE "it"

// The display name of the city.
#define DISPLAYED_CITY_NAME "Roma"
```
