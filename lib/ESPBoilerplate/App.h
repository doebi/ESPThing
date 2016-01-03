/*
 * ESP Boilerplate OS - App.h
 *
 */
#ifndef _APP_H
#define _APP_H

#include <Arduino.h>
#include "Types.h"

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
