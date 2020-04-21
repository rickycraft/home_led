#include <prism.h>

#pragma region vars

#define PIN 2
typedef NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> NEOMETHOD;  // uses GPIO3/RX
NEOMETHOD strip(LED_COUNT, PIN);  // PIN is ignored for ESP8266
NeoAnimationFX<NEOMETHOD> led_strip(strip);

bool update = false;
bool startup = true;
boolean light_state = false;             // current light state
uint8_t lux = 255;                       // current brightness
uint8_t effect = FX_MODE_RAINBOW_CYCLE;  // current effect
String effect_name = "rainbow";          // current effect name
uint8_t ha_speed = 128;                  // current ha effect speed
uint16_t speed = HA_SPEED;               // current effect speed
uint16_t color = WHITE;                  // current effect speed

#pragma endregion

#pragma region led strip

void set_state(bool new_state) {
    // if it powers on
    if (!light_state && new_state) led_strip.start();
    // if it powers off
    else if (light_state && !new_state) {
        set_lux(0);
        led_strip.stop();
    }
    light_state = new_state;
}

void set_lux(int new_lux) {
    // if out of bound do nothing
    if (new_lux < 0 || new_lux > 255) return;
    lux = new_lux;
    // avoid using max brightness
    if (new_lux > MAX_BRI) new_lux = MAX_BRI;

    led_strip.setBrightness(new_lux);
}

void set_speed(int new_speed) {
    ha_speed = new_speed;
    speed = (uint16_t)map(255 - new_speed, 0, 255, MAX_SPEED, MIN_SPEED);

    led_strip.setSpeed(speed);
}

void set_effect(String new_effect_name) {
    effect_name = new_effect_name;
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

    led_strip.setMode(effect);
}

#pragma endregion

#pragma region mqtt

bool decodeJson(String message) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return false;
    // state topic
    if (doc.containsKey("state"))
        if (strcmp(doc["state"], LIGHT_ON) == 0)
            set_state(true);
        else
            set_state(false);
    if (doc.containsKey("brightness")) set_lux(doc["brightness"].as<int>());
    if (doc.containsKey("effect")) set_effect(String(doc["effect"].as<char*>()));
    // white value is speed
    if (doc.containsKey("white_value")) set_speed(doc["white_value"].as<int>());

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

    mqttClient.publish(STATE_TOPIC, 2, true, message);
}

void onMqttConnect(bool sessionPresent) {
    Serial.println("connected");

    uint16_t packetIdSub = mqttClient.subscribe(COMMAND_TOPIC, 2);
    Serial.print("Subscribe to: ");
    Serial.println(COMMAND_TOPIC);

    sensor_setup();
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    String new_payload = String(payload).substring(0, len);
    Serial.printf("~ %s %s, qos: %d\n", topic, new_payload.c_str(), properties.qos);
    if (strcmp(topic, COMMAND_TOPIC) == 0) decodeJson(new_payload);
}

#pragma endregion  // mqtt

void update_alexa(uint8_t bri) {
    if (bri == 0)
        light_state = false;
    else {
        if (light_state)
            lux = bri;
        else
            light_state = true;
    }
    update = true;
}

void setup() {
    // safe startup
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    delay(1000);
    Serial.begin(19200);
    Serial.println();
    // mqtt setup
    mqttSetup(CLIENT_ID, STATE_TOPIC);
    // init ws2812fx
    led_strip.init();
    // init alexa
    alexa.addDevice(ALEXA_NAME, update_alexa);
    alexa.begin();
    // setup ota
    espOTA(HOSTNAME);
    // end of setup
    digitalWrite(BUILTIN_LED, HIGH);
}

void loop() {
    led_strip.service();
    ArduinoOTA.handle();
    sensor_read();
    alexa.loop();
}