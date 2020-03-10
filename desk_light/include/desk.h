#include <Arduino.h>

#include <Espalexa.h>
#include <MqttUtil.h>
#include <Util.h>
#include <WiFiUtil.h>
#include <mqtt_topics.h>

#define BUFFER_SIZE 5
#define FADE_DELAY 2
#define LED_PIN 12
#define BUTTON_PIN 0
#define ALEXA_NAME "scrivania"
#define HOSTNAME "desk"

WiFiUtil wifi;
MqttUtil mqtt;

void check_button();
void off();
void publish_state();
void update_led();
void update_alexa(uint8_t bri);
