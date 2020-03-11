#include <Arduino.h>

#include <ArduinoJson.h>
#include <Espalexa.h>
#include <MqttUtil.h>
#include <Util.h>
#include <WiFiUtil.h>

// vars
#define BUFFER_SIZE 5
#define FADE_DELAY 5
#define LED_PIN 15
#define JSON_BUFFER_SIZE JSON_OBJECT_SIZE(5)
// name
#define ALEXA_NAME "ambiente"
#define HOSTNAME "ambient"
// mqtt
#define CLIENT_ID "ambient_light"
#define STATE_TOPIC "led/state"
#define COMMAND_TOPIC "led/set"

WiFiUtil wifi;
MqttUtil mqtt;
Espalexa alexa;

void update_led();
void off();
void publish_state();
void update_led();
void update_alexa(uint8_t bri);
void callback(char *topic, byte *_payload, unsigned int _length);
bool decodeJson(String message);
