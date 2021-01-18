/**The MIT License (MIT)
Copyright (c) 2021 by Claudio Quaglia
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>          // ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            // Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     // Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <SPI.h>
#include <TFT_eSPI.h>

#include "settings.h"
#include "WeatherApi.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL "fonts/NotoSans10"
#define AA_FONT_MID "fonts/NotoSans14"
#define AA_FONT_LARGE "fonts/NotoSans34"

WeatherApiData currentWeather;
simpleDSTadjust dstAdjusted(StartRule, EndRule);

TFT_eSPI tft = TFT_eSPI();

void updateWeatherData();
void drawWiFiSignal();
void drawDateTime();
void drawProgress(uint8_t percentage, String text);

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void loadFonts() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }

  Serial.println("\r\nLittleFS available!");

  // ESP32 will crash if any of the fonts are missing
  bool font_missing = false;
  if (LittleFS.exists("/fonts/NotoSans10.vlw") == false) font_missing = true;
  if (LittleFS.exists("/fonts/NotoSans14.vlw") == false) font_missing = true;
  if (LittleFS.exists("/fonts/NotoSans34.vlw") == false) font_missing = true;

  if (font_missing) {
    Serial.println("\r\nFont missing in LittleFS, did you upload it?");
    while(1) yield();
  }
  else Serial.println("\r\nFonts found OK.");
}

void setup() {
  Serial.begin(115200);

  loadFonts();

  // Setup the TFT display
  tft.init();
  tft.setTextSize(1);  // Set font size
  tft.setRotation(0);  // portrait
  tft.setCursor(0, 0); // Set cursor at top left of screen
  tft.fillScreen(TFT_BLACK);
  tft.loadFont(AA_FONT_MID, LittleFS); // Must load the font first
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set the font colour AND the background colour so the anti-aliasing works

  tft.print("Display ready\n");

  // WiFiManager
  // Local initialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  // reset saved settings
  // wifiManager.resetSettings();

  // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }

  tft.print("\nConnected to WiFi with hostname:\n\n  " + hostname + "\n\n");
  tft.print("and with IP:\n\n  " + WiFi.localIP().toString());

  delay(5000);

  tft.fillScreen(TFT_BLACK);

  configTime(UTC_OFFSET * 3600, 0, NTP_SERVERS);
  // updateData();
}

void loop() {
  drawWiFiSignal();
  drawDateTime();
}

/***** Update helper *****/
void updateWeatherData() {
  WeatherApi *currentWeatherClient = new WeatherApi();
  currentWeatherClient->setMetric(IS_METRIC);
  currentWeatherClient->setLanguage(WEATHER_API_LANGUAGE);
  currentWeatherClient->update(&currentWeather, WEATHER_API_APP_ID, WEATHER_API_LOCATION);
  delete currentWeatherClient;
  currentWeatherClient = nullptr;
}

/***** Progress bar helper *****/
void drawProgress(uint8_t percentage, String text) {
  int progressBarSidePadding = 10;
  int progressBarHeight = 8;
  int maxProgressBarWidth = tft.width() - (progressBarSidePadding * 2);
  int varticalMiddleScreen = (tft.height() / 2)  - (progressBarHeight / 2);
  int fillProgress = (maxProgressBarWidth * percentage) / 100;

  int oldTextPadding = tft.getTextPadding();
  int newTextPadding = tft.textWidth(text, 1);

  tft.setTextPadding(oldTextPadding > newTextPadding ? oldTextPadding : newTextPadding);

  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(text, 120, varticalMiddleScreen - progressBarHeight);
  tft.drawRoundRect(progressBarSidePadding, varticalMiddleScreen, maxProgressBarWidth, progressBarHeight, 4, TFT_LIGHTGREY);
  tft.fillRoundRect(progressBarSidePadding, varticalMiddleScreen, fillProgress, progressBarHeight, 4, TFT_LIGHTGREY);
}

/***** Network signal indicator *****/
void wiFiSignalIndicator(int32_t s, int xpos, int ypos, uint16_t color) {
  tft.fillRect(xpos, ypos + 8, 2, 2, color);

  if (s < -88) tft.fillRect(xpos + 3, ypos + 9, 2, 4, TFT_DARKGREY);
  else tft.fillRect(xpos + 3, ypos + 6, 2, 4, color);

  if (s < -78) tft.fillRect(xpos + 6, ypos + 6, 2, 6, TFT_DARKGREY);
  else tft.fillRect(xpos + 6, ypos + 4, 2, 6, color);

  if (s <= -66) tft.fillRect(xpos + 9, ypos + 3, 2, 8, TFT_DARKGREY);
  else tft.fillRect(xpos + 9, ypos + 2, 2, 8, color);

  if (s < -55) tft.fillRect(xpos + 12, ypos, 2, 10, TFT_DARKGREY);
  else tft.fillRect(xpos + 12, ypos, 2, 10, color);
}

/***** Network quality indicator *****/
void wiFiQualityIndicator(int32_t s, int xpos, int ypos, uint16_t color) {
  int quality;

  if (s <= -100) quality = 0;
  else if (s >= -50) quality = 100;
  else quality = 2 * (s + 100);

  String buf = String(quality) + "%";
  tft.setTextColor(color, TFT_BLACK);
  tft.drawString(buf.c_str(), xpos, ypos);
}

/***** Draw WiFi quality *****/
void drawWiFiSignal() {
  uint16_t color;
  int32_t rssi = WiFi.RSSI();

  if (rssi < -88) color = TFT_RED;
  else if (rssi > -66) color = TFT_GREEN;
  else color = TFT_YELLOW;

  tft.loadFont(AA_FONT_SMALL, LittleFS);
  yield(); wiFiSignalIndicator(rssi, 200, 6, color);
  yield(); wiFiQualityIndicator(rssi, 218, 8, color);
  tft.unloadFont();
}

/***** Draw DateTime *****/
void drawDateTime() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  char time_str[11];
  char *dstAbbrev;
  time_t now = dstAdjusted.time(&dstAbbrev);
  struct tm * timeinfo = localtime (&now);

  String date = WDAY_NAMES[timeinfo->tm_wday] + " " + String(timeinfo->tm_mday) + " " + MONTH_NAMES[timeinfo->tm_mon] + " " + String(1900 + timeinfo->tm_year);

  tft.loadFont(AA_FONT_MID, LittleFS);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(date, 120, 13);
  tft.unloadFont();

  if (IS_STYLE_12HR) {
    int hour = (timeinfo->tm_hour+11)%12+1;  // take care of noon and midnight
    sprintf(time_str, "%2d:%02d\n",hour, timeinfo->tm_min);
  } else {
    sprintf(time_str, "%02d:%02d\n",timeinfo->tm_hour, timeinfo->tm_min);
  }


  tft.loadFont(AA_FONT_LARGE, LittleFS);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(String(time_str), 120, 45);
  tft.unloadFont();
}
