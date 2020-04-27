#include <Arduino.h>
#include <ArduinoJson.h>
#include <Espalexa.h>
#include <MqttBroker.h>
#include <OtaUpdatates.h>

// vars
#define BUFFER_SIZE 5
#define LED_PIN 5
#define JSON_BUFFER_SIZE JSON_OBJECT_SIZE(5)
// name
#define ALEXA_NAME "ambiente"
#define HOSTNAME "ambient"
// mqtt
#define CLIENT_ID "ambient_light"
#define STATE_TOPIC "led/state"
char* COMMAND_TOPIC = "led/set";

Espalexa alexa;

void update_alexa(uint8_t bri);
void publish_state();
void update_led();
