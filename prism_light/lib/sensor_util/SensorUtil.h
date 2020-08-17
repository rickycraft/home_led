#include <Adafruit_AHTX0.h>
#include <MqttBroker.h>

#define DELTA_TEMP 0.18
#define DELTA_HUMI 2.00

#define BUFFER_SIZE 5
// #define SENSOR_DEBUG

#define SECOND 1000
#define MINUTE 60 * SECOND

#ifdef SENSOR_DEBUG
#define READ_TIMEOUT 5 * SECOND
#else
#define READ_TIMEOUT 5 * MINUTE
#endif

// mqtt topic
#define TEMP_TOPIC "bedroom/temp"
#define HUMIDITY_TOPIC "bedroom/humidity"

void sensor_setup();
void sensor_read();
float absolute(float t);