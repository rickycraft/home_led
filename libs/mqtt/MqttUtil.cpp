#include <MqttUtil.h>

MqttUtil::MqttUtil(const char *client_id, const char *topic, bool restart)
    : client_id(client_id), topic(topic), restart(restart) {}

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

  if (client.connect(client_id, MQTT_USER, MQTT_PASS, topic, 0, true, "{\"state\": \"OFF\"}")) {
    client.subscribe(topic);
    attempts = 0;
    Serial.println("connected");
  } else
    Serial.println("failed");

  last_attempt = millis();
}

void MqttUtil::loop(void off()) {
  if (!client.loop()) {
    // turn off the first time
    if (attempts == 2) off();
    // try 10 times
    if (attempts < MAX_ATTEMPT && client.state() != MQTT_CONNECTED &&
        (millis() - last_attempt > MQTT_TIMEOUT)) {
      connect();
      attempts++;
    }
    // if restart on max attempt is true reset
    if (restart) reset();
  }
}

void MqttUtil::reset() {
  if (attempts >= MAX_ATTEMPT) ESP.restart();
}

void MqttUtil::publish(char *topic, const char *t) { client.publish(topic, t, true); }