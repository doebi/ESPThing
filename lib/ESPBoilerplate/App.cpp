/*
 * ESP Boilerplate OS - App.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"

ESPApplication::ESPApplication() {
    setup();
}

ESPApplication::~ESPApplication() {
}

void ESPApplication::setup() {
    WiFiMulti.addAP("DildoAP", "dildo123");
}

void ESPApplication::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFiMulti.run();
    }
}

void ESPApplication::addReceiver(String topic, void (*callback)(String topic, String message)){
}
