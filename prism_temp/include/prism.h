#include <Arduino.h>
#include <ArduinoJson.h>
#include <Espalexa.h>
#include <MqttBroker.h>
#include <NeoAnimationFX.h>
#include <OtaUpdatates.h>
#include <SensorUtil.h>

// VARS
#define LED_COUNT 60
#define LED_PIN 2
// bigger is slower
#define MAX_SPEED 2000
#define MIN_SPEED 15000
#define HA_SPEED 8576
#define MAX_BRI 255
#define ALEXA_NAME "prisma"
#define HOSTNAME "prism"
// json
#define JSON_BUFFER_SIZE JSON_OBJECT_SIZE(15)
#define CLIENT_ID "prism_light"
#define STATE_TOPIC "prism/state"
char* COMMAND_TOPIC = "prism/set";

typedef struct LedStatus {
    bool state;
    int lux;
    int effect_code;
    int speed;
};

typedef NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> NEOMETHOD;  // uses GPIO3/RX

void update_alexa(uint8_t bri);
void publish_state();
int get_effect_code();
void update_led();