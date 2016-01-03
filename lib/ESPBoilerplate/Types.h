/*
 * ESP Boilerplate OS - Types.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "MQTT.h"

class Receiver {
    public:
        Receiver(String t, void (*c)(MQTT::Publish pub)){
            topic = t;
            callback = c;
        }
        String topic;
        void (*callback)(MQTT::Publish pub);
};

class Sender {
    public:
        Sender(String t, String (*tp)(void)){
            topic = t;
            tryPublish = tp;
        }
        String topic;
        String (*tryPublish)(void);
};
