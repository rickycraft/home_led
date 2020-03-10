#include <prism.h>

#pragma region const
#define TOPIC_LEN 4
char *COMMAND_TOPIC[TOPIC_LEN] = {LIGHT_COMMAND_TOPIC, LUX_COMMAND_TOPIC, EFFECT_COMMAND_TOPIC,
                                  SPEED_COMMAND_TOPIC};

// vars
bool startup = true;
boolean light_state = false;             // current light state
uint8_t lux = 100;                       // current brightness
uint8_t effect = FX_MODE_RAINBOW_CYCLE;  // current effect
String effect_name = "rainbow";          // current effect name
uint8_t ha_speed = 50;                   // current ha effect speed
uint16_t speed = HA_SPEED;               // current effect speed
uint16_t color = WHITE;                  // current effect speed
#pragma endregion

#pragma region ws2812fx
void setEffect() {
  if (effect_name == "static") effect = FX_MODE_STATIC;
  if (effect_name == "rainbow") effect = FX_MODE_RAINBOW_CYCLE;
  if (effect_name == "wipe random") effect = FX_MODE_COLOR_WIPE_RANDOM;
  if (effect_name == "running lights") effect = FX_MODE_RUNNING_LIGHTS;
  if (effect_name == "chase rainbow") effect = FX_MODE_CHASE_RAINBOW;
  if (effect_name == "theatre chase rainbow") effect = FX_MODE_THEATER_CHASE_RAINBOW;
  if (effect_name == "color sweep random") effect = FX_MODE_COLOR_SWEEP_RANDOM;
  if (effect_name == "running random") effect = FX_MODE_RUNNING_RANDOM;
  if (effect_name == "bicolor chase") effect = FX_MODE_BICOLOR_CHASE;
  if (effect_name == "tricolor chase") effect = FX_MODE_TRICOLOR_CHASE;
}

void update_led() {
  if (light_state) {
    ws2812fx.stop();
    ws2812fx.setBrightness(lux);
    ws2812fx.setSegment(0, 0, LED_COUNT - 1, effect, WHITE, speed, GAMMA);
    ws2812fx.start();
  } else {
    ws2812fx.setBrightness(0);
    ws2812fx.stop();
  }
  publish_state();
}

void off() {
  light_state = false;
  update_led();
}
#pragma endregion

#pragma region mqtt
void publish_state() {
  // light state
  mqtt.publish(LIGHT_STATE_TOPIC, (light_state) ? LIGHT_ON : LIGHT_OFF);
  // light brightness
  mqtt.publish(LUX_STATE_TOPIC, lux);
  // current effect_name
  mqtt.publish(EFFECT_STATE_TOPIC, stringToChar(effect_name));
  // current effect speed
  mqtt.publish(SPEED_STATE_TOPIC, ha_speed);
}

// function called when a MQTT message arrived
void callback(char *topic, byte *_payload, unsigned int _length) {
  // handle state topic
  if (strcmp(topic, LIGHT_COMMAND_TOPIC) == 0) {
    String payload = byte_concat(_payload, _length);
    prettyPrint(LIGHT_COMMAND_TOPIC, payload);
    light_state = (payload == LIGHT_ON) ? true : false;
  }
  // handle brightness topic
  else if (strcmp(topic, LUX_COMMAND_TOPIC) == 0) {
    uint16_t brightness = int_byte_concat(_payload, _length);
    prettyPrint(LUX_COMMAND_TOPIC, brightness);
    if (brightness < 0 || brightness > 255) return;
    lux = brightness;
  }
  // handle effect topic
  else if (strcmp(topic, EFFECT_COMMAND_TOPIC) == 0) {
    String payload = byte_concat(_payload, _length);
    prettyPrint(EFFECT_COMMAND_TOPIC, payload);
    // make sure payload is not empty
    if (payload.length() > 1) {
      effect_name = payload;
      setEffect();
    }
    // handle speed topic
  } else if (strcmp(topic, SPEED_COMMAND_TOPIC) == 0) {
    uint8_t new_speed = int_byte_concat(_payload, _length);
    if (new_speed < 0 || new_speed > 100) return;
    ha_speed = new_speed;
    speed = (uint16_t)map(100 - new_speed, 0, 100, MAX_SPEED, MIN_SPEED);
    prettyPrint(SPEED_COMMAND_TOPIC, (int)speed);
  }
  if (startup && light_state) {
    light_state = false;
    startup = false;
    Serial.println("~ startup done");
  }
  update_led();
}
#pragma endregion

#pragma region alexa
Espalexa alexa;

void update_alexa(uint8_t bri) {
  mqtt.reset();
  if (bri == 0)
    light_state = false;
  else {
    if (light_state)
      lux = bri;
    else
      light_state = true;
  }
  prettyPrint("alexa/lux", lux);
  prettyPrint("alexa/switch", (light_state) ? "ON" : "OFF");
  update_led();
}
#pragma endregion

void setup() {
  // safe startup
  delay(1000);
  Serial.begin(115200);
  Serial.printf("\nplatform.io version\n");
  // init wifi
  wifi = WiFiUtil(HOSTNAME);
  yield();
  // init the MQTT connection
  mqtt = MqttUtil(CLIENT_ID, COMMAND_TOPIC, TOPIC_LEN, LIGHT_STATE_TOPIC);
  mqtt.start(callback);
  yield();
  // init dht22
  dht = MyDHT(&mqtt);
  // init ws2812fx
  ws2812fx.init();
  // init alexa
  alexa.addDevice(ALEXA_NAME, update_alexa);
  alexa.begin();
  // end of setup
  Serial.println();
}

void loop() {
  wifi.loop(off);
  mqtt.loop(off);
  ws2812fx.service();
  alexa.loop();
  dht.read();
}