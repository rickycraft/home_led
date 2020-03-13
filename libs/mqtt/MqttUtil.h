#ifndef MQTT_UTIL
#define MQTT_UTIL

#include <Credentials.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LIGHT_ON "ON"
#define LIGHT_OFF "OFF"
#define MQTT_TIMEOUT 10000
#define MAX_ATTEMPT 10

class MqttUtil {
 public:
  MqttUtil(){};
  MqttUtil(const char *client_id, const char *topic, bool restart);
  void start(void cb(char *, byte *, unsigned int));
  void connect();
  void publish(char *topic, const char *t);
  void loop(void off());
  void reset();

 private:
  const char *client_id;
  const char *topic;
  bool restart;
  unsigned long last_attempt = 0;
  unsigned int attempts = 0;
  WiFiClient espClient;
  PubSubClient client;
};

#endif  // MQTT_UTIL