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
int PIN_STATE = 0;
/* ~userconf */

void callback(MQTT::Publish pub) {
    // do something with that pub
}

String tryPublish() {
    if (digitalRead(PIN) != PIN_STATE) {
        PIN_STATE = !PIN_STATE;
        if (PIN_STATE){
            return "YAY";
        } else {
            return "NEY";
        }
    }
}

void setup() {
    App.addReceiver("foo", callback);
    App.addSender("bar", tryPublish);
}

void loop() {
    App.loop();
}
