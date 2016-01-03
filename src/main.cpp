/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */

#include "Arduino.h"

/* config */
#define WIFI_ENABLED 1
#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"

#define MQTT_ENABLED 1
#define MQTT_HOST "mqtt.devlol.org"

String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";
/* ~config */

#include "App.h"

// #include "PubSubClient.h"
// 
// 
// WiFiClient w;
// PubSubClient MQTTClient(w, "mqtt.devlol.org");
// 
// void mqtt_callback(const MQTT::Publish& pub) {
//     String topic = pub.topic();
//     String message = pub.payload_string();
//     if (topic != MQTT_BASEPATH + "pong") {
//         MQTTClient.publish(MQTT_BASEPATH + "log", message);
//     }
// }

ESPApplication App;

/* userconf */
int PIN = 0;
/* ~userconf */

Sender_t * senders;
Receiver_t * receivers;

void callback(String topic, String message) {
}

void setup() {
    App.addReceiver("foo", callback);
}

void loop() {
    App.loop();

    // if (MQTTClient.connected()) {
    //   MQTTClient.loop();
    // } else {
    //   if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
    //     MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
    //       MQTTClient.set_callback(mqtt_callback);
    //       MQTTClient.subscribe(MQTT_BASEPATH + "#");
    //   }
    // }
}
