#include <SensorUtil.h>

AHT10 aht;

// last sensor state
float t, h;
// buffers
char t_buff[BUFFER_SIZE], h_buff[BUFFER_SIZE];
unsigned long last_update = millis();

void sensor_setup() {
    if (!aht.begin(SDA, SCL)) Serial.println("failed begin of ATH10");
};

void sensor_read() {
    if ((millis() - last_update) < READ_TIMEOUT) return;
    aht.readRawData();

    float aht_t = aht.readTemperature(false);
    if (absolute(t - aht_t) > DELTA_TEMP) {
        t = aht_t;
        snprintf(t_buff, BUFFER_SIZE, "%.1f", t);
        mqttClient.publish(TEMP_TOPIC, 1, false, t_buff);
    }

    float aht_h = aht.readHumidity(false);
    if (absolute(h - aht_t) > DELTA_HUMI) {
        h = aht_h;
        snprintf(h_buff, BUFFER_SIZE, "%.0f", h);
        mqttClient.publish(HUMIDITY_TOPIC, 1, false, h_buff);
    }

    last_update = millis();
};

float absolute(float t) { return (t < 0) ? -1 * t : t; }