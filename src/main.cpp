/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "Arduino.h"
#include "WiFiManager.h"
#include "PubSubClient.h"

String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";

void mqtt_callback(const MQTT::Publish& pub) {
}

WiFiManager wm;
WiFiClient wc;
PubSubClient MQTTClient(wc, "mqtt.devlol.org");

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

class ESPApplication {
    private:
        void setup() {
            log("setup");
            wm.addAP("/dev/lol", "4dprinter");
            wm.addAP("dit.net", "Faid4Youters$8Thurning2Prats!");
            // wm.addAP("DildoAP", "dildo123");
        }

    public:
        ESPApplication(){
            setup();
        }

        ~ESPApplication(){
        }

        void loop(){
            wm.loop();

            if (WiFi.status() == WL_CONNECTED) {
                if (MQTTClient.connected()) {
                    MQTTClient.loop();
                } else {
                    log("connecting MQTT");
                    if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
                        log("MQTT connected");
                        MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
                        MQTTClient.set_callback(mqtt_callback);
                        MQTTClient.subscribe(MQTT_BASEPATH + "#");
                    }
                }
            }
        }

        void log(String message){
            if (Serial) {
                Serial.println(message);
            }
        }
};

ESPApplication App;

void setup() {
    // For Debugging
    Serial.begin(115200);
}

void loop() {
    App.loop();
}
