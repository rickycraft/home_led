#ifndef MQTT_BROKER
#define MQTT_BROKER

#include <AsyncMqttClient.h>
#include <Credentials.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

#define WILL_PAYLOAD "{\"state\": \"OFF\"}"
#define LIGHT_ON "ON"
#define LIGHT_OFF "OFF"

extern AsyncMqttClient mqttClient;

void connectToWifi();
void onWifiConnect(const WiFiEventStationModeGotIP& event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);
void mqttSetup(const char* client_id, const char* will_topic);

#endif  // MQTT_BROKER