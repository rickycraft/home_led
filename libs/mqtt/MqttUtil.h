#ifndef MQTT_UTIL
#define MQTT_UTIL

#include <Credentials.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LIGHT_ON "ON"
#define LIGHT_OFF "OFF"
#define MQTT_TIMEOUT 10000
#define MAX_ATTEMPT 2

class MqttUtil {
 public:
  MqttUtil(){};
  MqttUtil(char *client_id, char *topics[], uint16_t len, char *will_topic);
  void start(void cb(char *, byte *, unsigned int));
  void connect();
  void publish(char *topic, int t);
  void publish(char *topic, const char *t);
  void loop(void off());
  void reset();

 private:
  char *client_id;
  char *will_topic;
  char **topics;
  uint16_t topics_len;
  unsigned long last_attempt = 0;
  unsigned int attempts = 0;
  WiFiClient espClient;
  PubSubClient client;
};

#endif  // MQTT_UTIL