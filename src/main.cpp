/*
 * ESP Boilerplate OS - main.cpp
 *
 */
#include "App.h"

/* config */
#define WIFI_ENABLED 1
#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"

#define MQTT_ENABLED 1
#define MQTT_HOST "mqtt.devlol.org"
#define MQTT_BASEPATH "doebi/test/"
/* ~config */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

ESPApplication App;

/* userconf */
int PIN = 0;
/* ~userconf */

Sender_t * senders;
Receiver_t * receivers;

void callback(String topic, String message) {
    Serial.println(topic + " = " + message);
}

void setup() {
    App.addReceiver("foo", callback);

    WiFiMulti.addAP("DildoAP", "dildo123");
}

void loop() {
    App.loop();

    if (WiFi.status() != WL_CONNECTED) {
        WiFiMulti.run();
    }
}
