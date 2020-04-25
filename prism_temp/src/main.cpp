#include <prism.h>

#pragma region vars

NEOMETHOD strip(LED_COUNT);  // PIN is ignored for ESP8266
NeoAnimationFX<NEOMETHOD> led_strip(strip);
Espalexa alexa;

String effect_name = "rainbow";  // current effect name
uint8_t ha_speed = 128;          // current ha effect speed
uint16_t color = WHITE;          // current color

// force state update
bool update = false;
LedStatus current_status = {false, 0, FX_MODE_STATIC, 0};
LedStatus new_status = {false, 255, FX_MODE_RAINBOW_CYCLE, HA_SPEED};

#pragma endregion

#pragma region led strip

int get_effect_code() {
    if (effect_name == "static") return FX_MODE_STATIC;
    if (effect_name == "cycle") return FX_MODE_RAINBOW;
    if (effect_name == "rainbow") return FX_MODE_RAINBOW_CYCLE;
    if (effect_name == "wipe random") return FX_MODE_COLOR_WIPE_RANDOM;
    if (effect_name == "running lights") return FX_MODE_RUNNING_LIGHTS;
    if (effect_name == "chase rainbow") return FX_MODE_CHASE_RAINBOW;
    if (effect_name == "theatre chase rainbow") return FX_MODE_THEATER_CHASE_RAINBOW;
    if (effect_name == "color sweep random") return FX_MODE_COLOR_SWEEP_RANDOM;
    if (effect_name == "running random") return FX_MODE_RUNNING_RANDOM;
    if (effect_name == "bicolor chase") return FX_MODE_BICOLOR_CHASE;
    if (effect_name == "tricolor chase") return FX_MODE_TRICOLOR_CHASE;
    // just in case
    return FX_MODE_RAINBOW_CYCLE;
}

void update_led() {
    // update status
    if (current_status.state != new_status.state) {
        current_status.state = new_status.state;
        // if power on
        if (current_status.state) {
            // force lux update
            current_status.lux = new_status.lux;
            led_strip.setBrightness(current_status.lux);
            led_strip.start();
        } else {
            // if power off
            led_strip.setBrightness(0);
            led_strip.stop();
        }
    }
    // update lux
    if (current_status.lux != new_status.lux) {
        current_status.lux = new_status.lux;
        led_strip.setBrightness(current_status.lux);
    }
    // update speed
    if (current_status.speed != new_status.speed) {
        current_status.speed = new_status.speed;
        led_strip.setSpeed(current_status.speed);
    }
    // update effect
    if (current_status.effect_code != new_status.effect_code) {
        current_status.effect_code = new_status.effect_code;
        led_strip.setMode(current_status.effect_code);
    }
    update = false;
    publish_state();
}
#pragma endregion

#pragma region network

bool decodeJson(String message) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return false;
    // state topic
    if (doc.containsKey("state"))
        new_status.state = (strcmp(doc["state"], LIGHT_ON) == 0) ? true : false;
    // brightness topic
    if (doc.containsKey("brightness")) new_status.lux = doc["brightness"].as<int>();
    if (doc.containsKey("effect")) {
        effect_name = String(doc["effect"].as<char*>());
        new_status.effect_code = get_effect_code();
    }
    // white value is speed
    if (doc.containsKey("white_value")) {
        ha_speed = doc["white_value"].as<int>();
        new_status.speed = (uint16_t)map(255 - ha_speed, 0, 255, MAX_SPEED, MIN_SPEED);
    }
    update = true;
    return true;
}

void publish_state() {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    doc["state"] = (new_status.state) ? LIGHT_ON : LIGHT_OFF;
    doc["brightness"] = new_status.lux;
    doc["effect"] = effect_name.c_str();
    doc["white_value"] = ha_speed;

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
    // sensor setup
    sensor_setup();
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    String new_payload = String(payload).substring(0, len);
    Serial.printf("~ %s %s, qos: %d\n", topic, new_payload.c_str(), properties.qos);
    if (strcmp(topic, COMMAND_TOPIC) == 0) decodeJson(new_payload);
}

void update_alexa(uint8_t bri) {
    Serial.printf("~ alexa bri: %d\n", bri);
    if (bri == 0)
        new_status.state = false;
    else {
        new_status.lux = bri;
        new_status.state = true;
    }
    update = true;
}

#pragma endregion  // network

void setup() {
    // safe startup
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    delay(1000);
    Serial.begin(19200);
    Serial.println();
    // mqtt setup
    mqttSetup(CLIENT_ID, STATE_TOPIC);
    // wifi connect
    connectToWifi();
    // init ws2812fx
    led_strip.init();
    led_strip.setColor(WHITE);
    // end of setup
    digitalWrite(BUILTIN_LED, HIGH);
}

void loop() {
    if (update) update_led();
    led_strip.service();
    ArduinoOTA.handle();
    sensor_read();
    alexa.loop();
}