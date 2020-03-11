#include <Arduino.h>

#include <ArduinoJson.h>
#include <Espalexa.h>
#include <MqttUtil.h>
#include <Util.h>
#include <WiFiUtil.h>

// pin
#define LED_PIN 12
#define BUTTON_PIN 0
#define BUFFER_SIZE 5
#define FADE_DELAY 2
// json
#define JSON_BUFFER_SIZE JSON_OBJECT_SIZE(5)
// name
#define ALEXA_NAME "scrivania"
#define HOSTNAME "desk"
// mqtt
#define CLIENT_ID "desk_light"
#define STATE_TOPIC "desk/state"
#define COMMAND_TOPIC "desk/set"

WiFiUtil wifi;
MqttUtil mqtt;
Espalexa alexa;

void check_button();
void off();
void publish_state();
void update_led();
void update_alexa(uint8_t bri);
bool decodeJson(String message);
