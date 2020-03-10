#ifndef MY_DHT
#define MY_DHT

#include <DHT.h>
#include <MqttUtil.h>
#include <Util.h>

#define READ_TIMEOUT 60000
#define DHTTYPE DHT22
#define DELTA_TEMP 0.45
#define DELTA_HUMI 1.50
#define DHTPIN 13
#define BUFFER_SIZE 5

// mqtt topic
#define TEMP_TOPIC "bedroom/temp"
#define HUMIDITY_TOPIC "bedroom/humidity"

class MyDHT : private DHT {
 public:
  MyDHT();
  MyDHT(MqttUtil *mqtt);
  void read();

 private:
  float temp;
  float humi;
  unsigned long last_update;
  MqttUtil *mqtt;
};

#endif  // MYDHT
