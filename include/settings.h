#include <simpleDSTadjust.h>
#include <Adafruit_Sensor.h>

// Setup
#define HOSTNAME "ESP8266-OTA-"

// Customize DHT sensor setting. Replace with your connected PIN
#define DHT_PIN 16          // D0

// Uncomment whatever type you're using!
#define DHT_TYPE DHT11      // DHT 11
//#define DHT_TYPE DHT22    // DHT 22  (AM2302), AM2321
//#define DHT_TYPE DHT21    // DHT 21  (AM2301)


// Weatherapi setup
// Sign up here to get an API key: https://www.weatherapi.com/my/
const String WEATHER_API_APP_ID = "REPLACE";
/*
Go to https://www.weatherapi.com/api-explorer.aspx and search for a location-
 */
const String WEATHER_API_LOCATION = "Rome";
/*
Arabic -> ar, Bulgarian -> bg, Catalan -> ca, Czech -> cz, German -> de, Greek -> el,
English -> en, Persian (Farsi) -> fa, Finnish -> fi, French -> fr, Galician -> gl,
Croatian -> hr, Hungarian -> hu, Italian -> it, Japanese -> ja, Korean -> kr,
Latvian -> la, Lithuanian -> lt, Macedonian -> mk, Dutch -> nl, Polish -> pl,
Portuguese -> pt, Romanian -> ro, Russian -> ru, Swedish -> se, Slovak -> sk,
Slovenian -> sl, Spanish -> es, Turkish -> tr, Ukrainian -> ua, Vietnamese -> vi,
Chinese Simplified -> zh_cn, Chinese Traditional -> zh_tw.
*/
const String WEATHER_API_LANGUAGE = "it";

// Adjust according to your language
const String DISPLAYED_CITY_NAME = "Roma";
const String WDAY_NAMES[] = {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"};
const String MONTH_NAMES[] = {"Gen", "Feb", "Mar", "Apr", "Mag", "Giu", "Lug", "Ago", "Set", "Ott", "Nov", "Dic"};

#define UTC_OFFSET +1
// change for different NTP (time servers)
#define NTP_SERVERS "0.ch.pool.ntp.org", "1.ch.pool.ntp.org", "2.ch.pool.ntp.org"
// #define NTP_SERVERS "us.pool.ntp.org", "time.nist.gov", "pool.ntp.org"

// August 1st, 2018
#define NTP_MIN_VALID_EPOCH 1533081600

// Central European Summer Time = UTC/GMT +2 hours
struct dstRule StartRule = {"CEST", Last, Sun, Mar, 2, 3600};

// Central European Time = UTC/GMT +1 hour
struct dstRule EndRule = {"CET", Last, Sun, Oct, 2, 0};

// Update every 10 minutes
const int UPDATE_INTERVAL_SECS = 10 * 60;

// Use °C or °F
const bool IS_METRIC = true;

// 12 or 24 horus
const bool IS_STYLE_12HR = false;
