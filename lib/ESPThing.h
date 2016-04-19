/*
 * ESP Boilerplate OS - ESPThing.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#ifndef ESPTHING_H_
#define ESPTHING_H_

#include "PubSubClient.h"
#include <vector>

class Input {
    public:
        Input(){};
        Input(String t, void (*c)(const MQTT::Publish& pub)){
            topic = t;
            callback = c;
        }
        String topic;
        void (*callback)(const MQTT::Publish& pub);
};

class Output {
    public:
        Output(){};
        Output(String t, void (*l)(String * msg)){
            topic = t;
            loop = l;
            interval = 0;
        };
        Output(String t, void (*l)(String * msg), int i){
            topic = t;
            loop = l;
            interval = i;
        }
        String topic;
        void (*loop)(String * msg);
        int interval;
        int last_run = 0;
};

class ESPThing {
    public:
        ESPThing();
        ~ESPThing();

        void loop();

        void addOutput(const Output &o);

        void addInput(const Input &i);
        
    private:
        bool fallback = false;
        int last_connect = 0;
        std::vector<Output> outputs;
        std::vector<Input> inputs;
        
        void setup();
        void log(String message);
        void mqtt_loop();
        void server_loop();
        void handleRoot();
        void handleNotFound();
        void mqtt_callback(const MQTT::Publish& pub);
};

#endif /* ESPTHING_H_ */
