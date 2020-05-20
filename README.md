# Home LED

Wifi connected led strip, using mqtt  
Support for alexa

## Purpose

Connect various type of led strip to [Home Assistant](https://www.home-assistant.io)

## Library used

-   [ArduinoJson](https://arduinojson.org) for parsing json from HomeAssistant
-   [MqttClient](https://github.com/marvinroger/async-mqtt-client) to connect to HomeAssistant
-   [NeoAnimationFX](https://github.com/debsahu/NeoAnimationFX) for ws2812 led strip (many effects already included)
-   [Espalexa](https://github.com/Aircoookie/Espalexa) for Alexa integration
-   Credentials.h is just an header with wifi and mqtt credentials

### License

[MIT](https://choosealicense.com/licenses/mit/)
