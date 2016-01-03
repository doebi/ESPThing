/*
 * ESP Boilerplate OS - App.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"

#include <functional>
#include "MQTT.h"

void ext_mqtt_callback(const MQTT::Publish& pub){
}

String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";

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
            /*
            std::function<void(const MQTT::Publish&)> callback = [=](const MQTT::Publish& publish) { this->mqtt_callback(publish); };
            */
            MQTTClient.set_callback(ext_mqtt_callback);
            MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
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
    Sender* s = new Sender(topic, tryPublish);
    senders.add(s);
}

void ESPApplication::mqtt_callback(const MQTT::Publish& pub) {
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
