#include <Arduino.h>
#include <ArduinoJson.h>
#include <Espalexa.h>
#include <MqttBroker.h>
#include <OtaUpdatates.h>

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
char* COMMAND_TOPIC = "desk/set";

Espalexa alexa;

void update_alexa(uint8_t bri);
void publish_state();
void update_led();
