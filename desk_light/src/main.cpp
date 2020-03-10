#include <desk.h>

#define TOPIC_LEN 2
char *COMMAND_TOPIC[2] = {LIGHT_COMMAND_TOPIC, LUX_COMMAND_TOPIC};

boolean light_state = false;
uint8_t lux = 100;
uint8_t _lux = lux;
uint8_t buttonStatus;

void off() {
  light_state = false;
  update_led();
}

void update_led() {
  int fadeTo = (light_state) ? lux : 0;
  // increase brightness
  while (fadeTo > _lux) {
    _lux++;
    analogWrite(LED_PIN, _lux);
    delay(FADE_DELAY);
  }
  // decrease brightness
  while (fadeTo < _lux) {
    _lux--;
    analogWrite(LED_PIN, _lux);
    delay(FADE_DELAY);
  }
  publish_state();
}

void check_button() {
  // if button changes status
  if (digitalRead(BUTTON_PIN) != buttonStatus) {
    buttonStatus = digitalRead(BUTTON_PIN);
    light_state = !light_state;  // switch state
    // if on set to 255
    if (light_state) lux = 255;
    update_led();
  }
}

void callback(char *topic, byte *_payload, unsigned int _length) {
  // handle switch topic
  if (strcmp(topic, LIGHT_COMMAND_TOPIC) == 0) {
    String payload = byte_concat(_payload, _length);
    prettyPrint(LIGHT_COMMAND_TOPIC, payload);
    light_state = (payload == LIGHT_ON) ? true : false;
  }
  // handle brightness topic
  else if (strcmp(topic, LUX_COMMAND_TOPIC) == 0) {
    uint8_t brightness = int_byte_concat(_payload, _length);
    prettyPrint(LUX_COMMAND_TOPIC, brightness);
    if (brightness < 0 || brightness > 255) return;
    if (brightness == 0)
      light_state = false;
    else
      lux = brightness;
  }
  update_led();
}

void publish_state() {
  // light state
  mqtt.publish(LIGHT_STATE_TOPIC, (light_state) ? LIGHT_ON : LIGHT_OFF);
  // light brightness
  mqtt.publish(LUX_STATE_TOPIC, lux);
}

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

void setup() {
  // safe startup
  delay(1000);
  Serial.begin(115200);
  // init the led
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  analogWrite(LED_PIN, 150);
  buttonStatus = digitalRead(BUTTON_PIN);
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
  // end setup
  analogWrite(LED_PIN, 0);
}

void loop() {
  wifi.loop(off);
  mqtt.loop(off);
  alexa.loop();
  check_button();
}