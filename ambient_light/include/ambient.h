#include <Arduino.h>

#include <Espalexa.h>
#include <MqttUtil.h>
#include <Util.h>
#include <WiFiUtil.h>
#include <mqtt_topics.h>

#define BUFFER_SIZE 5
#define FADE_DELAY 5
#define LED_PIN 15
#define ALEXA_NAME "ambiente"
#define HOSTNAME "ambient"

WiFiUtil wifi;
MqttUtil mqtt;

void update_led();
void off();
void publish_state();
void update_led();
