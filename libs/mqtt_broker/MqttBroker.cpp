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
    // if ((int8_t)reason == 0) ESP.reset();
    if (WiFi.isConnected()) mqttReconnectTimer.once(2, connectToMqtt);
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
#ifdef ACK_LOG
    Serial.println("sub ack");
#endif
}

void onMqttUnsubscribe(uint16_t packetId) {
#ifdef ACK_LOG
    Serial.println("unsub ack");
#endif
}

void onMqttPublish(uint16_t packetId) {
#ifdef ACK_LOG
    Serial.println("pub ack");
#endif
}

void mqttSetup(const char* client_id, const char* will_topic) {
    wifiConnectHandler = WiFi.onStationModeGotIP(_onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    mqttClient.onConnect(_onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(_onMqttMessage);
    mqttClient.onPublish(onMqttPublish);

    mqttClient.setClientId(client_id);
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
    mqttClient.setServer(SERVER_IP, SERVER_PORT);
    mqttClient.setWill(will_topic, 0, true, WILL_PAYLOAD);
}

void _onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties,
                    size_t len, size_t index, size_t total) {
    String new_payload = String(payload).substring(0, len);
    Serial.printf("~ %s %s, qos: %d\n", topic, new_payload.c_str(), properties.qos);
    if (strcmp(topic, COMMAND_TOPIC) == 0) {
        if (!decodeJson(new_payload)) Serial.println("failed to decode json");
    } else
        onMqttMessage(topic, new_payload);
}

void _onWifiConnect(const WiFiEventStationModeGotIP& event) {
    Serial.println("connected");
    // call func
    onWifiConnect();
    // connect to mqtt
    connectToMqtt();
}

void _onMqttConnect(bool sessionPresent) {
    Serial.println("connected");
    // subscribe to mqtt topic
    uint16_t packetIdSub = mqttClient.subscribe(COMMAND_TOPIC, 2);
    Serial.print("Subscribe to: ");
    Serial.println(COMMAND_TOPIC);
    // call func
    onMqttConnect();
}