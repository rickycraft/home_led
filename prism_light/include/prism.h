#include <Arduino.h>

#include <ArduinoJson.h>
#include <Espalexa.h>
#include <WS2812FX.h>

#include <MqttUtil.h>
#include <MyDHT.h>
#include <Util.h>
#include <WiFiUtil.h>
#include <prism_mqtt.h>

// VARS
#define LED_COUNT 60
#define LED_PIN 4
// bigger is slower
#define MAX_SPEED 2000
#define MIN_SPEED 15000
#define HA_SPEED 8500
#define ALEXA_NAME "prisma"
#define HOSTNAME "prism"
// json
#define JSON_BUFFER_SIZE JSON_OBJECT_SIZE(15)

MyDHT dht;
WiFiUtil wifi;
MqttUtil mqtt;
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void off();
void update_led();
void update_alexa(uint8_t bri);
void publish_state();
void callback(char *topic, byte *_payload, unsigned int _length);
void set_effect();
void encodeJson();
bool decodeJson(String message);