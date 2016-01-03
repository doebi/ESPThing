/*
 * ESP Boilerplate OS - App.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#ifndef _APP_H
#define _APP_H

#include "Types.h"
#include "Arduino.h"
#include "WiFi.h"

class ESPApplication {
    private:
        Sender_t * senders;
        Receiver_t * receivers;

    public:
        ESPApplication();
        ~ESPApplication();
        void setup();
        void loop();
        void addReceiver(String topic, void (*callback)(String topic, String message));
};

#endif
