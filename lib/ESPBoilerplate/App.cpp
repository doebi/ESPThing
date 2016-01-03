/*
 * ESP Boilerplate OS - App.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "App.h"

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
          MQTTClient.set_callback(process_mqtt);
          MQTTClient.subscribe(MQTT_BASEPATH + "#");
      }
    }
}

void ESPApplication::addReceiver(String topic, void (*callback)(String topic, String message)){
    Receiver* r = new Receiver(topic, callback);
    receivers.add(r);
}

void ESPApplication::process_mqtt(const MQTT::Publish& pub) {
    String topic = pub.topic();
    String message = pub.payload_string();

    Receiver* r;
    for(int i = 0; i < receivers.size(); i++){
        r = receivers.get(i);
        MQTTClient.publish("doebi/test/log", r->topic);
    }
}
