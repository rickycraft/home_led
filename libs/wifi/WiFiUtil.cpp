#include "WiFiUtil.h"

WiFiUtil::WiFiUtil(char *hostname) {
  connect();
  espOTA(hostname);
}

void WiFiUtil::connect() {
  Serial.printf("WiFi STATUS (%d)\n", WiFi.status());
  Serial.printf("Connecting to %s", WIFI_SSID);

  if (WiFi.status() == WL_CONNECTED)
    WiFi.reconnect();
  else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }
  yield();
  unsigned long wifi_start_time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if ((millis() - wifi_start_time) > WIFI_TIMEOUT) {
      Serial.println("wifi timeout excedeed resetting");
      ESP.restart();
    }
  }
  Serial.printf("connected\nIP address: ");
  Serial.println(WiFi.localIP());
};

void WiFiUtil::loop(void off()) {
  if (WiFi.status() != WL_CONNECTED) {
    off();
    connect();
  }
  ArduinoOTA.handle();
}

void WiFiUtil::espOTA(char *hostname) {
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
};