/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"

/* config */
#define WIFI_ENABLED 1
#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"

#define MQTT_ENABLED 1
#define MQTT_HOST "mqtt.devlol.org"

/* ~config */

ESPApplication App;

/* userconf */
int PIN = 0;
/* ~userconf */

void callback(String topic, String message) {
}

void setup() {
    App.addReceiver("foo", callback);
}

void loop() {
    App.loop();
}
