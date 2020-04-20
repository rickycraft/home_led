#include <MqttBroker.h>

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
    Serial.println("connected");
    connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    Serial.println("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach();  // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
    Serial.print("Connecting to MQTT...");
    mqttClient.connect();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.print("Disconnected from MQTT, code: ");
    Serial.println((int8_t)reason);
    if ((int8_t)reason == 0) ESP.reset();
    if (WiFi.isConnected()) mqttReconnectTimer.once(2, connectToMqtt);
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) { Serial.println("sub ack"); }

void onMqttUnsubscribe(uint16_t packetId) { Serial.println("unsub ack"); }

void onMqttPublish(uint16_t packetId) {
    //
    Serial.println("pub ack");
}

void mqttSetup(const char* client_id, const char* will_topic) {
    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);

    mqttClient.setClientId(client_id);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
    mqttClient.setServer(SERVER_IP, SERVER_PORT);
    mqttClient.setWill(will_topic, 0, true, WILL_PAYLOAD);

    connectToWifi();
}