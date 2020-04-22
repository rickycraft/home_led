#include <SensorUtil.h>

AHT10 aht;

// last sensor state
float t = 0, h = 0;
// buffers
char t_buff[BUFFER_SIZE], h_buff[BUFFER_SIZE];
unsigned long last_update = millis();
bool did_setup = false;

void sensor_setup() {
    Serial.print("Setting up sensor...");
    if (!aht.begin(SDA, SCL))
        Serial.println("failed begin of ATH10");
    else {
        did_setup = true;
        Serial.println("done");
    }
};

void sensor_read() {
    if ((millis() - last_update) < READ_TIMEOUT || !did_setup) return;
    aht.readRawData();

    float aht_t = aht.readTemperature(false);
    if (absolute(t - aht_t) > DELTA_TEMP) {
        t = aht_t;
        Serial.printf("~ temp %.1f°C\n", t);
        snprintf(t_buff, BUFFER_SIZE, "%.1f", t);
        mqttClient.publish(TEMP_TOPIC, 2, true, t_buff);
    }

    float aht_h = aht.readHumidity(false);
    if (absolute(h - aht_h) > DELTA_HUMI) {
        h = aht_h;
        Serial.printf("~ humi %.0f%c \n", h, '%');
        snprintf(h_buff, BUFFER_SIZE, "%.0f", h);
        mqttClient.publish(HUMIDITY_TOPIC, 2, true, h_buff);
    }

    last_update = millis();
};

float absolute(float t) { return (t < 0) ? -1 * t : t; }