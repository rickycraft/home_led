#include <Adafruit_AHT10.h>
#include <MqttBroker.h>

#define READ_TIMEOUT 600000

#define DELTA_TEMP 0.30
#define DELTA_HUMI 1.50

#define BUFFER_SIZE 5
// #define SENSOR_DEBUG_TIME

// mqtt topic
#define TEMP_TOPIC "bedroom/temp"
#define HUMIDITY_TOPIC "bedroom/humidity"

void sensor_setup();
void sensor_read();
float absolute(float t);