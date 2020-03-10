#include <MyDHT.h>

MyDHT::MyDHT() : DHT(DHTPIN, DHTTYPE) { Serial.println("called default constructor"); };

MyDHT::MyDHT(MqttUtil *mqtt) : DHT(DHTPIN, DHTTYPE) {
  MyDHT::mqtt = mqtt;
  last_update = millis();
  begin();
}

void MyDHT::read() {
  if ((millis() - last_update) < READ_TIMEOUT) return;
  last_update = millis();

  float h = readHumidity();
  float t = readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("CHECK SENSOR");
    return;
  }

  char buff[BUFFER_SIZE];
  float delta_t = t - temp;
  float delta_h = h - humi;

  // if delta temp is grater than threshold publish new value
  if (absolute(delta_t) > DELTA_TEMP) {
    snprintf(buff, BUFFER_SIZE, "%.1f", t);
    mqtt->publish(TEMP_TOPIC, buff);
    temp = t;  // update last temp
    prettyPrint("temp ", t, "°C");
  }

  // if delta humi is grater than threshold publish new value
  if (absolute(delta_h) > DELTA_HUMI) {
    snprintf(buff, BUFFER_SIZE, "%.0f", h);
    mqtt->publish(HUMIDITY_TOPIC, buff);
    humi = h;  // update last temp
    prettyPrint("humidity ", h, "%");
  }
}