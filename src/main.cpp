/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "Arduino.h"
#include "WiFiManager.h"
#include "PubSubClient.h"
#include "ESP8266WebServer.h"

#define CONNECT_TIME 1 //minutes until we go to fallback
#define RECONNECT_TIME 1 //minutes until we try to reconnect

String NODE_DOMAIN = "devlol";
String NODE_ID = WiFi.macAddress();
String MQTT_BASEPATH = NODE_DOMAIN + "/" + NODE_ID + "/";

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
        // Serial.println("AP up - handle client");
        server.handleClient();
    }
}

void handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleNotFound() {
    server.send(404, "text/html", "File Not Found");
}

typedef struct {
    std::vector<WifiAPlist_t> APlist;
    std::string friendlyName;
    std::string mqttServer;
    std::string mqttDomain;
} app_config_t;

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
        bool fallback = false;
        int last_connect = 0;
        void setup() {
            log("setup");
            wm.addAP("/dev/lol", "4dprinter");
            wm.addAP("dit.net", "Faid4Youters$8Thurning2Prats!");
            wm.addAP("DildoAP", "dildo123");
            wm.addAP("Flughafenfeuerwehr", "gwdmilfeuerwehr");

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
            if (WiFi.status() == WL_CONNECTED) {
                last_connect = millis(); // we still got connection (yay)
                mqtt_loop();
            } else {
                if (fallback) {
                    // in fallback mode we handle server
                    if (last_connect + (1000 * 60 * RECONNECT_TIME) > millis()) {
                        server_loop();
                    } else {
                        log("RECONNECT");
                        fallback = false;
                        last_connect = millis(); // reset timeout 
                    }
                } else {
                    // if not in fallback mode try to connect to network
                    if (last_connect + (1000 * 60 * CONNECT_TIME) > millis()) {
                        wm.AutoConnect();
                        delay(500);
                    } else {
                        // timeout reached we go to fallback
                        log("TIMEOUT");
                        fallback = true;
                        wm.FallbackAP();
                        last_connect = millis(); // reset timeout 
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
