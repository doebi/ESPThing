/*
 * ESP Boilerplate OS - App.cpp
 *
 */
#include "App.h"

ESPApplication::ESPApplication() {
    setup();
}

ESPApplication::~ESPApplication() {
}

void ESPApplication::setup() {
}

void ESPApplication::loop() {
}

void ESPApplication::addReceiver(String topic, void (*callback)(String topic, String message)){
}
