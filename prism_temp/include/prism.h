#include <Arduino.h>
#include <ArduinoJson.h>
#include <Espalexa.h>
#include <MqttBroker.h>
#include <OtaUpdatates.h>
#include <WS2812FX.h>

// VARS
#define LED_COUNT 53
#define LED_PIN 0
// bigger is slower
#define MAX_SPEED 2000
#define MIN_SPEED 15000
#define HA_SPEED 8576
#define ALEXA_NAME "prisma"
#define HOSTNAME "prism"
// json
#define JSON_BUFFER_SIZE JSON_OBJECT_SIZE(15)
#define CLIENT_ID "prism_light"
#define STATE_TOPIC "prism/state"
#define COMMAND_TOPIC "prism/set"

Espalexa alexa;
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void update_alexa(uint8_t bri);
bool decodeJson(String message);
void publish_state();
void set_effect();
void update_led();
