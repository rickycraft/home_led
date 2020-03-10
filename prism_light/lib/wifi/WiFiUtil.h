#ifndef WIFI_UTIL
#define WIFI_UTIL

#include <ArduinoOTA.h>
#include <Credentials.h>
#include <ESP8266WiFi.h>

#define WIFI_TIMEOUT 30000

class WiFiUtil {
 public:
  WiFiUtil(){};
  WiFiUtil(char *hostname);
  void connect();
  void loop(void off());

 private:
  void espOTA(char *hostname);
};

#endif  // WIFI_UTIL