#include <MqttUtil.h>

// WiFiClient espClient;
// PubSubClient client(espClient);

MqttUtil::MqttUtil(char *client_id, char *topics[], uint16_t len, char *will_topic)
    : client_id(client_id), topics(topics), topics_len(len), will_topic(will_topic) {}

void MqttUtil::start(void cb(char *t, byte *p, unsigned int l)) {
  // init the client
  espClient = WiFiClient();
  client = PubSubClient(espClient);
  // start the server
  client.setServer(SERVER_IP, SERVER_PORT);
  client.setCallback(cb);
  connect();
}

void MqttUtil::connect() {
  Serial.print("Attempting MQTT connection...");
  yield();
  if (client.connect(client_id, MQTT_USER, MQTT_PASS, will_topic, 0, false, "OFF")) {
    attempts = 0;
    for (int i = 0; i < topics_len; i++) client.subscribe(topics[i]);
    Serial.println("connected");
  } else {
    Serial.println("failed");
  }
  last_attempt = millis();
}

void MqttUtil::loop(void off()) {
  if (!client.loop()) {
    // turn off the first time
    if (client.state() != MQTT_CONNECT_FAILED) off();
    // try 10 times
    if (attempts < MAX_ATTEMPT && client.state() != MQTT_CONNECTED &&
        (millis() - last_attempt > MQTT_TIMEOUT)) {
      connect();
      attempts++;
    }
  }
}

void MqttUtil::reset() {
  if (attempts >= MAX_ATTEMPT) {
    Serial.println("mqtt reset");
    ESP.restart();
  }
}

void MqttUtil::publish(char *topic, int t) {
  char buff[5];
  snprintf(buff, 5, "%d", t);
  publish(topic, buff);
}

void MqttUtil::publish(char *topic, const char *t) { client.publish(topic, t, true); }