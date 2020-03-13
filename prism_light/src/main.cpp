#include <prism.h>

#pragma region const
// vars
bool startup = true;
boolean light_state = false;             // current light state
uint8_t lux = 255;                       // current brightness
uint8_t effect = FX_MODE_RAINBOW_CYCLE;  // current effect
String effect_name = "rainbow";          // current effect name
uint8_t ha_speed = 128;                  // current ha effect speed
uint16_t speed = HA_SPEED;               // current effect speed
uint16_t color = WHITE;                  // current effect speed
#pragma endregion

#pragma region ws2812fx
void set_effect() {
  if (effect_name == "static") effect = FX_MODE_STATIC;
  if (effect_name == "cycle") effect = FX_MODE_RAINBOW;
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
    ws2812fx.setBrightness(lux);
    ws2812fx.setSegment(0, 0, LED_COUNT - 1, effect, BLUE, speed, GAMMA);
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

bool decodeJson(String message) {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) return false;
  prettyPrint("json", message);
  // state topic
  if (doc.containsKey("state"))
    if (strcmp(doc["state"], LIGHT_ON) == 0) {
      if (!light_state) ws2812fx.start();
      light_state = true;
    } else
      light_state = false;
  if (doc.containsKey("brightness")) lux = doc["brightness"].as<int>();
  if (doc.containsKey("effect")) {
    effect_name = String(doc["effect"].as<char *>());
    set_effect();
  }
  // white value is speed
  if (doc.containsKey("white_value")) {
    int new_speed = doc["white_value"].as<int>();
    ha_speed = new_speed;
    speed = (uint16_t)map(255 - new_speed, 0, 255, MAX_SPEED, MIN_SPEED);
  }
  return true;
}

void publish_state() {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  doc["state"] = (light_state) ? LIGHT_ON : LIGHT_OFF;
  doc["brightness"] = lux;
  doc["effect"] = effect_name.c_str();
  doc["white_value"] = ha_speed;

  int len = measureJson(doc) + 1;
  char message[len];
  serializeJson(doc, message, len);

  mqtt.publish(STATE_TOPIC, message);
}
// function called when a MQTT message arrived
void callback(char *topic, byte *_payload, unsigned int _length) {
  // handle state topic
  if (strcmp(topic, COMMAND_TOPIC) == 0) decodeJson(byte_concat(_payload, _length));
  update_led();
}

#pragma endregion  // mqtt

void update_alexa(uint8_t bri) {
  if (bri == 0)
    light_state = false;
  else {
    if (light_state)
      lux = bri;
    else {
      light_state = true;
      ws2812fx.start();
    }
  }

  prettyPrint("alexa/lux", lux);
  prettyPrint("alexa/switch ", (light_state) ? "ON" : "OFF");
  update_led();
}

void setup() {
  // safe startup
  delay(1000);
  Serial.begin(115200);
  Serial.printf("\nplatform.io version\n");
  // init wifi
  wifi = WiFiUtil(HOSTNAME);
  yield();
  // init the MQTT connection
  mqtt = MqttUtil(CLIENT_ID, COMMAND_TOPIC, true);
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