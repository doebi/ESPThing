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

class ESPApplication {
    private:
        void setup();
        LinkedList<Sender*> senders = LinkedList<Sender*>();
        LinkedList<Receiver*> receivers = LinkedList<Receiver*>();

        ESP8266WiFiMulti WiFiMulti;
        WiFiClient w;

    public:
        ESPApplication();
        ~ESPApplication();
        void loop();
        void addReceiver(String topic, void (*callback)(MQTT::Publish pub));
        void addSender(String topic, String (*tryPublish)(void));

        PubSubClient MQTTClient;

};

#endif
