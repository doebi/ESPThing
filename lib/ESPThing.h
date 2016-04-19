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
        Input(String t, void (*c)(const MQTT::Publish& pub));
        String topic;
        void (*callback)(const MQTT::Publish& pub);
};

class Output {
    public:
        Output(){};
        Output(String t, void (*l)(String * msg));
        Output(String t, void (*l)(String * msg), int i);
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

        /*! equals to calling thingPublish([...]) */
        void addOutput(const Output &o);

        /*! equals to calling thingSubscribe([...]) */
        void addInput(const Input &i);

        /*!
         * Convenience method for subscribing to an internal (device related) topic 
         * for which the given callback is called.
         * 
         * \param t the topic specified as relative path (the configured topic prefix is prepended)
         * \param c the callback function
         */
        void thingSubscribe(String t, void (*c)(const MQTT::Publish& pub));

        /*!
         * Convenience method for publishing to an internal (device related) topic.
         * 
         * \param t the topic specified as relative path (the configured topic prefix is prepended)
         * \param l the callback function which may set a new message to be published
         */
        void thingPublish(String t, void (*l)(String * msg));

        /*!
         * Convenience method for publishing to an internal (device related) topic.
         * 
         * \param t the topic specified as relative path (the configured topic prefix is prepended)
         * \param l the callback function which may set a new message to be published
         * \param i the interval in milliseconds after which the callback function is called
         */
        void thingPublish(String t, void (*l)(String * msg), int i);
        
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

        void subscribe(String t, void (*c)(const MQTT::Publish& pub), bool internal);
        void publish(String t, void (*l)(String * msg), int i, bool internal);
};

#endif /* ESPTHING_H_ */
