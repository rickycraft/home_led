#include <ambient.h>

#pragma region vars
#define TOPIC_LEN 2
char *COMMAND_TOPIC[TOPIC_LEN] = {LIGHT_COMMAND_TOPIC, LUX_COMMAND_TOPIC};

boolean light_state = false;
uint8_t lux = 100;  // setted lux
#pragma endregion   // vars

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
#pragma endregion  // alexa

#pragma region mqtt
void publish_state() {
  // light state
  mqtt.publish(LIGHT_STATE_TOPIC, (light_state) ? LIGHT_ON : LIGHT_OFF);
  // light brightness
  mqtt.publish(LUX_STATE_TOPIC, lux);
}

void callback(char *topic, byte *payload, unsigned int length) {
  // handle switch topic
  if (strcmp(topic, LIGHT_COMMAND_TOPIC) == 0) {
    String p = byte_concat(payload, length);
    prettyPrint(topic, p);
    // test if the payload is equal to "ON" or "OFF"
    light_state = (p == LIGHT_ON) ? true : false;
  }
  // handle brightness topic
  else if (strcmp(topic, LUX_COMMAND_TOPIC) == 0) {
    uint8_t brightness = int_byte_concat(payload, length);
    prettyPrint(topic, brightness);
    if (brightness < 0 || brightness > 255) return;
    if (brightness == 0)
      light_state = false;
    else
      lux = brightness;
  }
  update_led();
}
#pragma endregion  // mqtt

#pragma region light
void update_led() {
  if (light_state) {
    if (lux == 255)
      digitalWrite(LED_PIN, HIGH);
    else
      analogWrite(LED_PIN, lux);
  } else
    digitalWrite(LED_PIN, LOW);
  publish_state();
}

void off() {
  light_state = false;
  update_led();
}
#pragma endregion  // light

void setup() {
  // safe startup
  delay(1000);
  Serial.begin(115200);
  // init the led
  pinMode(LED_PIN, OUTPUT);
  // init the WiFi connection
  wifi = WiFiUtil(HOSTNAME);
  yield();
  // init the MQTT connection
  mqtt = MqttUtil(CLIENT_ID, COMMAND_TOPIC, TOPIC_LEN, LIGHT_STATE_TOPIC);
  mqtt.start(callback);
  yield();
  // init alexa
  alexa.addDevice(ALEXA_NAME, update_alexa);
  alexa.begin();
  // end of setup
  analogWrite(LED_PIN, lux);
}

void loop() {
  wifi.loop(off);
  mqtt.loop(off);
  alexa.loop();
}