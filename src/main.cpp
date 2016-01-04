/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"
ESPApplication App;

void setup() {
    // For Debugging
    Serial.begin(115200);
}

void loop() {
    App.loop();
}
