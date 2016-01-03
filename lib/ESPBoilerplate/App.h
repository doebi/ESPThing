/*
 * ESP Boilerplate OS - App.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#ifndef _APP_H
#define _APP_H

#include <Arduino.h>
#include "Types.h"
#include "MQTT.h"
#include <LinkedList.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "PubSubClient.h"

String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";

class ESPApplication {
    private:
        void setup();
        //LinkedList<Sender_t*> senders = LinkedList<Sender_t*>();
        LinkedList<Receiver*> receivers = LinkedList<Receiver*>();

        ESP8266WiFiMulti WiFiMulti;
        WiFiClient w;

    public:
        ESPApplication();
        ~ESPApplication();
        void loop();
        void process_mqtt(const MQTT::Publish& pub);
        void addReceiver(String topic, void (*callback)(String topic, String message));

        PubSubClient MQTTClient;

};

#endif
