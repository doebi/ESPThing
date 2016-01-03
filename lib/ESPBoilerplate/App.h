/*
 * ESP Boilerplate OS - App.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#ifndef _APP_H
#define _APP_H

#include <Arduino.h>
#include "Types.h"
#include <LinkedList.h>

class ESPApplication {
    private:
        void setup();
        //LinkedList<Sender_t*> senders = LinkedList<Sender_t*>();
        LinkedList<Receiver*> receivers = LinkedList<Receiver*>();

    public:
        ESPApplication();
        ~ESPApplication();
        void loop();
        void addReceiver(String topic, void (*callback)(String topic, String message));
};

#endif
