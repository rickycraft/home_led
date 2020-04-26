#include <ambient.h>

boolean light_state = false;
uint8_t lux = 100;  // setted lux

void update_alexa(uint8_t bri) {
    mqtt.reset();
    // if does't want to turn off and the light was on
    lux = (bri != 0 && light_state) ? bri : lux;
    // if bri == 0 just turn off
    light_state = (bri == 0) ? false : true;
    update_led();
}

#pragma region mqtt

bool decodeJson(String message) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return false;
    Serial.println(message);
    // state topic
    if (doc.containsKey("state"))
        light_state = (strcmp(doc["state"], LIGHT_ON) == 0) ? true : false;
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
#pragma endregion  // mqtt

#pragma region light
void update_led() {
    if (light_state) {
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
    pinMode(BUILTIN_LED, OUTPUT);
    analogWriteRange(255);
    analogWriteFreq(110);
    digitalWrite(BUILTIN_LED, LOW);
    delay(1000);
    Serial.begin(115200);
    // init the led
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    // init the WiFi connection
    wifi = WiFiUtil(HOSTNAME);
    yield();
    // init the MQTT connection
    mqtt = MqttUtil(CLIENT_ID, COMMAND_TOPIC, true);
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
}