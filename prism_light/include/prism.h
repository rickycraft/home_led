#include <Arduino.h>

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
#define MAX_SPEED 1000
#define MIN_SPEED 10000
#define HA_SPEED 4500
#define ALEXA_NAME "prisma"
#define HOSTNAME "prism"

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