/*
 * ESP Boilerplate OS - App.h
 *
 */
#ifndef _APP_H
#define  _APP_H

#include <Arduino.h>

class ESPApplication {
    private:

    public:
        ESPApplication();
        void setup();
        void loop();
};

#else
extern ESPApplication App;
#endif
