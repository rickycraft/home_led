#ifndef MQTT_TOPIC_LIST
#define MQTT_TOPIC_LIST

#define CLIENT_ID "prism_light"

#define LIGHT_STATE_TOPIC "prism/state"
#define LIGHT_COMMAND_TOPIC "prism/switch"

#define LUX_COMMAND_TOPIC "prism/lux"
#define LUX_STATE_TOPIC "prism/lux/state"

#define EFFECT_COMMAND_TOPIC "prism/effect"
#define EFFECT_STATE_TOPIC "prism/effect/state"

#define SPEED_COMMAND_TOPIC "prism/speed"
#define SPEED_STATE_TOPIC "prism/speed/state"

#define JSON_STATE_TOPIC "prism/json/state"
#define JSON_COMMAND_TOPIC "prism/json/set"

#endif  // MQTT_TOPIC_LIST