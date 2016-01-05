/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "Arduino.h"
#include "WiFiManager.h"
#include "PubSubClient.h"
#include "ESP8266WebServer.h"

String NODE_ID = "test";
String MQTT_BASEPATH = "doebi/" + NODE_ID + "/";

void mqtt_callback(const MQTT::Publish& pub) {
}

WiFiManager wm;
WiFiClient wc;
PubSubClient MQTTClient(wc, "mqtt.devlol.org");
ESP8266WebServer server(80);

void mqtt_loop() {
    if (MQTTClient.connected()) {
        MQTTClient.loop();
    } else {
        Serial.println("connecting MQTT");
        if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
            Serial.println("MQTT connected");
            MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
            MQTTClient.set_callback(mqtt_callback);
            MQTTClient.subscribe(MQTT_BASEPATH + "#");
        }
    }
}

void server_loop() {
    if (WiFi.softAPIP()){
        Serial.println("AP up - handle client");
        server.handleClient();
    }
}

void handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleNotFound() {
    server.send(404, "text/html", "File Not Found");
}

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
            wm.addAP("DildoAP", "dildo123");

            // configure server
            server.on("/", handleRoot);
            server.onNotFound(handleNotFound);
            server.begin();
        }

    public:
        ESPApplication(){
            setup();
        }

        ~ESPApplication(){
        }

        void loop(){
            switch (WiFi.status()) {
                case WL_CONNECTED:
                    // connect mqtt
                    mqtt_loop();
                    break;
                case WL_DISCONNECTED:
                    // try to AutoConnect
                    wm.AutoConnect();
                    break;
                case WL_IDLE_STATUS:
                    // handle webserver
                    server_loop();
                    break;
                case WL_CONNECT_FAILED:
                    // try again? or go with Fallback?
                    wm.FallbackAP();
                    break;
                case WL_NO_SSID_AVAIL:
                    // still running
                    delay(100);
                    break;
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
