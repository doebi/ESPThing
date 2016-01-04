/*
 * ESP Boilerplate OS - App.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#ifndef _APP_H
#define _APP_H

#include <Arduino.h>
#include "Types.h"
#include <ESP8266WiFiMulti.h>

class ESPApplication {
    
    private:
        void setup();
        ESP8266WiFiMulti WiFiMulti;

    public:
        ESPApplication();
        ~ESPApplication();
        void loop();
        void log(String message);

};

#endif
