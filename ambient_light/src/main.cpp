#include <ambient.h>

boolean light_state = false;
uint8_t lux = 100;  // setted lux

#pragma region networking

bool decodeJson(String message) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return false;
    // state topic
    if (doc.containsKey("state"))
        light_state = (strcmp(doc["state"], LIGHT_ON) == 0) ? true : false;
    if (doc.containsKey("brightness")) lux = doc["brightness"].as<int>();
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

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
    Serial.println("connected");
    // init alexa
    alexa.addDevice(ALEXA_NAME, update_alexa);
    Serial.print("Alexa begin ");
    Serial.println((alexa.begin()) ? "success" : "failed");
    // setup ota
    espOTA(HOSTNAME);
    // connect to mqtt
    connectToMqtt();
}

void onMqttConnect(bool sessionPresent) {
    Serial.println("connected");
    // subscribe to mqtt topic
    uint16_t packetIdSub = mqttClient.subscribe(COMMAND_TOPIC, 2);
    Serial.print("Subscribe to: ");
    Serial.println(COMMAND_TOPIC);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    String new_payload = String(payload).substring(0, len);
    Serial.printf("~ %s %s, qos: %d\n", topic, new_payload.c_str(), properties.qos);
    if (strcmp(topic, COMMAND_TOPIC) == 0) {
        if (decodeJson(new_payload)) update_led();
    }
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
}

void update_led() {
    analogWrite(LED_PIN, (light_state) ? lux : 0);
    publish_state();
}

void setup() {
    // safe startup
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    Serial.begin(19200);
    Serial.println();
    // mqtt setup
    mqttSetup(CLIENT_ID, STATE_TOPIC);
    // wifi connect
    connectToWifi();
    // end of setup
    digitalWrite(BUILTIN_LED, HIGH);
}

void loop() {
    ArduinoOTA.handle();
    alexa.loop();
}