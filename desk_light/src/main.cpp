#include <desk.h>

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
  mqtt.reset();
  // if button changes status
  if (digitalRead(BUTTON_PIN) != buttonStatus) {
    buttonStatus = digitalRead(BUTTON_PIN);
    // switch state
    light_state = !light_state;
    // if on set to 255
    if (light_state) lux = 255;
    update_led();
  }
}

bool decodeJson(String message) {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) return false;
  prettyPrint("json", message);
  // state topic
  if (doc.containsKey("state")) light_state = (strcmp(doc["state"], LIGHT_ON) == 0) ? true : false;
  if (doc.containsKey("brightness")) lux = doc["brightness"].as<int>();
  return true;
}

void callback(char *topic, byte *_payload, unsigned int _length) {
  // handle state topic
  if (strcmp(topic, COMMAND_TOPIC) == 0) decodeJson(byte_concat(_payload, _length));
  update_led();
}

void publish_state() {
  StaticJsonDocument<JSON_BUFFER_SIZE> doc;
  doc["state"] = (light_state) ? LIGHT_ON : LIGHT_OFF;
  doc["brightness"] = lux;

  int len = measureJson(doc) + 1;
  char message[len];
  serializeJson(doc, message, len);

  mqtt.publish(STATE_TOPIC, message);
}

void update_alexa(uint8_t bri) {
  // if does't want to turn off and the light was on
  lux = (bri != 0 && light_state) ? bri : lux;
  // if bri == 0 just turn off
  light_state = (bri == 0) ? false : true;

  prettyPrint("alexa/lux", lux);
  prettyPrint("alexa/switch", (light_state) ? "ON" : "OFF");
  update_led();
}

void setup() {
  // safe startup
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  delay(1000);
  Serial.begin(115200);  // init the led
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonStatus = digitalRead(BUTTON_PIN);
  // init the WiFi connection
  wifi = WiFiUtil(HOSTNAME);
  yield();
  // init the MQTT connection
  mqtt = MqttUtil(CLIENT_ID, COMMAND_TOPIC, false);
  mqtt.start(callback);
  yield();
  // init alexa
  alexa.addDevice(ALEXA_NAME, update_alexa);
  alexa.begin();
  // end setup
  digitalWrite(BUILTIN_LED, HIGH);
}

void loop() {
  wifi.loop(off);
  mqtt.loop(off);
  alexa.loop();
  check_button();
}