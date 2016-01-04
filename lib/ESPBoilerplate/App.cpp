/*
 * ESP Boilerplate OS - App.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"
String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";

void mqtt_callback(const MQTT::Publish& pub) {
}

ESPApplication::ESPApplication() {
    setup();
}

ESPApplication::~ESPApplication() {
}

void ESPApplication::setup() {
    log("setup");
    WiFiMulti.addAP("/dev/lol", "4dprinter");
    // PubSubClient MQTTClient(w, "mqtt.devlol.org");
}

void ESPApplication::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        log("reconnecting WiFi...");
        if ( WiFiMulti.run() == WL_CONNECTED){
            log("WiFi connected!");
        }
    }

    /*

    if (MQTTClient.connected()) {
        MQTTClient.loop();
    } else {
        if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
            MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
            MQTTClient.set_callback(mqtt_callback);
            MQTTClient.subscribe(MQTT_BASEPATH + "#");
        }
    }

    for (int i = 0; i < senders.size(); ++i) {
        MQTTClient.publish(MQTT_BASEPATH + "log", "oida");        
    }
    */
}

void ESPApplication::log(String message) {
    if (Serial) {
        Serial.println(message);
    }
}
