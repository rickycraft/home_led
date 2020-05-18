#include <SensorUtil.h>

Adafruit_AHT10 aht;

// last sensor state
float t = 0, h = 0;
bool is_rising = true;
// buffers
char t_buff[BUFFER_SIZE], h_buff[BUFFER_SIZE];
unsigned long last_update;
bool did_setup = false;

void sensor_setup() {
#ifdef SENSOR_DEBUG
    last_update = READ_TIMEOUT;
    Serial.println("DEBUG: sensor in debug mode");
#else
    last_update = millis();
#endif
    Serial.print("Setting up sensor...");
    if (!aht.begin())
        Serial.println("failed begin of ATH10");
    else {
        did_setup = true;
        Serial.println("done");
    }
};

void sensor_read() {
    if ((millis() - last_update) < READ_TIMEOUT || !did_setup) return;
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data

    float aht_t = temp.temperature;
    if (absolute(t - aht_t) > DELTA_TEMP) {
#ifdef SENSOR_DEBUG
        Serial.printf("DEBUG: old temp %.1f°C, new temp %.1f°C, %s\n", t, aht_t,
                      (is_rising) ? "rising" : "dropping");
#endif
        if ((aht_t - t > 0 && is_rising) || (aht_t - t < 0 && !is_rising)) {
            snprintf(t_buff, BUFFER_SIZE, "%.1f", aht_t);
#ifdef SENSOR_DEBUG
            Serial.printf("~ temp %s°C\n", t_buff);
#endif
            mqttClient.publish(TEMP_TOPIC, 2, true, t_buff);
        }
        is_rising = (aht_t - t > 0);
        t = aht_t;
    }

    float aht_h = humidity.relative_humidity;
    if (absolute(h - aht_h) > DELTA_HUMI) {
        h = aht_h;
        snprintf(h_buff, BUFFER_SIZE, "%.0f", h);
#ifdef SENSOR_DEBUG
        Serial.printf("~ humi %s%c \n", h_buff, '%');
#endif
        mqttClient.publish(HUMIDITY_TOPIC, 2, true, h_buff);
    }

    last_update = millis();
};

float absolute(float t) { return (t < 0) ? -1 * t : t; }