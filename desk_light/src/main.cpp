#include <desk.h>

boolean light_state = false;
uint8_t lux = 100;
uint8_t _lux = lux;
uint8_t buttonStatus;
Espalexa alexa;

void update_led() {
    publish_state();
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
}

void check_button() {
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
    // state topic
    if (doc.containsKey("state"))
        light_state = (strcmp(doc["state"], LIGHT_ON) == 0) ? true : false;
    if (doc.containsKey("brightness")) lux = doc["brightness"].as<int>();
    update_led();
    return true;
}

void publish_state() {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    doc["state"] = (light_state) ? LIGHT_ON : LIGHT_OFF;
    doc["brightness"] = lux;

    int len = measureJson(doc) + 1;
    char message[len];
    serializeJson(doc, message, len);

    mqttClient.publish(STATE_TOPIC, 2, true, message);
}

void onWifiConnect() {
    // init alexa
    alexa.addDevice(ALEXA_NAME, update_alexa);
    Serial.print("Alexa begin ");
    Serial.println((alexa.begin()) ? "success" : "failed");
    // setup ota
    espOTA(HOSTNAME);
}

void onMqttConnect() { digitalWrite(BUILTIN_LED, HIGH); }

void update_alexa(uint8_t bri) {
    // if does't want to turn off and the light was on
    lux = (bri != 0 && light_state) ? bri : lux;
    // if bri == 0 just turn off
    light_state = (bri == 0) ? false : true;
    update_led();
}

void setup() {
    // safe startup
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    buttonStatus = digitalRead(BUTTON_PIN);
    analogWriteRange(255);
    analogWriteFreq(120);
    delay(1000);
    Serial.begin(19200);
    Serial.println();
    // mqtt setup
    mqttSetup(CLIENT_ID, STATE_TOPIC);
    // wifi connect
    connectToWifi();
}

void loop() {
    ArduinoOTA.handle();
    alexa.loop();
    check_button();
}