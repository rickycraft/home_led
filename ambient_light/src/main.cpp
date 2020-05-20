#include <ambient.h>

boolean light_state = false;
boolean is_morning = false;
unsigned long last_update = 0;
uint8_t lux = 100;  // setted lux
Espalexa alexa;

#pragma region networking

bool decodeJson(String message) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return false;
    // state topic
    if (doc.containsKey("state"))
        light_state = (strcmp(doc["state"], LIGHT_ON) == 0) ? true : false;
    if (doc.containsKey("brightness")) lux = doc["brightness"].as<int>();
    // update led
    if (is_morning) set_morning(false);
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

void onMqttConnect() {
    mqttClient.subscribe(MORNING_COMMAND_TOPIC, 2);
    digitalWrite(BUILTIN_LED, HIGH);
};

void onMqttMessage(char *topic, String payload) {
    if (strcmp(topic, MORNING_COMMAND_TOPIC) == 0) {
        if (payload == "ON" && !light_state) {
            set_morning(true);
            lux = 0;
            light_state = true;
        } else
            set_morning(false);
    };
}

#pragma endregion  // networking

void update_alexa(uint8_t bri) {
    Serial.printf("~ alexa bri: %d\n", bri);
    if (bri == 0)
        light_state = false;
    else {
        lux = bri;
        light_state = true;
    }
    update_led();
}

void morning() {
    if (millis() - last_update > MORNING_DELAY) {
        last_update = millis();
        if (lux < 255) {
            lux++;
            update_led();
        } else
            set_morning(false);
    }
}

void set_morning(bool state) {
    is_morning = state;
    mqttClient.publish(MORNING_STATE_TOPIC, 2, false, (state) ? LIGHT_ON : LIGHT_OFF);
}

void update_led() {
    if (light_state) {
        if (lux > 250)
            digitalWrite(LED_PIN, HIGH);
        else
            analogWrite(LED_PIN, lux);
    } else
        digitalWrite(LED_PIN, LOW);

    publish_state();
}

void setup() {
    // safe startup
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    analogWriteRange(255);
    analogWriteFreq(18000);
    delay(1000);
    Serial.begin(19200);
    Serial.println();
    // mqtt setup
    mqttSetup(CLIENT_ID, STATE_TOPIC);
    // wifi connect
    connectToWifi();
}

void loop() {
    if (is_morning) morning();
    ArduinoOTA.handle();
    alexa.loop();
}