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
    WiFiMulti.addAP("dit.net", "Faid4Youters$8Thurning2Prats!");
    WiFiMulti.addAP("DildoAP", "dildo123");
}

void ESPApplication::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        log("reconnecting WiFi...");
        if ( WiFiMulti.run() == WL_CONNECTED){
            log("WiFi connected!");
        }
    }
}

void ESPApplication::log(String message) {
    if (Serial) {
        Serial.println(message);
    }
}
