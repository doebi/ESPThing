/*
 * ESP Boilerplate OS - App.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"

String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";

void mqtt_callback(const MQTT::Publish& pub) {
    /*
    String topic = pub.topic();
    String message = pub.payload_string();

    Receiver* r;
    for(int i = 0; i < receivers.size(); i++){
        r = receivers.get(i);
        MQTTClient.publish("doebi/test/log", r->topic);
    }
    */
}

ESPApplication::ESPApplication() {
    setup();
}

ESPApplication::~ESPApplication() {
}

void ESPApplication::setup() {
    PubSubClient MQTTClient(w, "mqtt.devlol.org");
    WiFiMulti.addAP("DildoAP", "dildo123");
}

void ESPApplication::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        WiFiMulti.run();
    }

    if (MQTTClient.connected()) {
        MQTTClient.loop();
    } else {
        if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
            MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
            MQTTClient.set_callback(mqtt_callback);
            MQTTClient.subscribe(MQTT_BASEPATH + "#");
        }
    }

    for (int i = 0; i < senders.size(); ++i) {
        MQTTClient.publish(MQTT_BASEPATH + "log", "oida");        
    }
}

void ESPApplication::addReceiver(String topic, void (*callback)(MQTT::Publish pub)){
    Receiver* r = new Receiver(topic, callback);
    receivers.add(r);
}

void ESPApplication::addSender(String topic, String (*tryPublish)(void)) {
}
