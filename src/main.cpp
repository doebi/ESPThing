/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "Arduino.h"
#include "WiFiManager.h"
#include "PubSubClient.h"
#include "ESP8266WebServer.h"
#include <json/json.h>

typedef struct {
    std::vector<WifiAPlist_t> APlist;
    String friendlyName;
    String mqttServer;
    String mqttDomain;
} app_config_t;

app_config_t config = {
    {
        { "/dev/lol", "4dprinter" },
        { "dit.net", "Faid4Youters$8Thurning2Prats!" },
        { "Flughafenfeuerwehr", "gwdmilfeuerwehr" }
    },
    "MS3000",
    "mqtt.devlol.org",
    "devlol/things"
};

#define CONNECT_TIME 1   // minutes until we go to fallback
#define RECONNECT_TIME 15 // minutes until we try to reconnect

String NODE_ID = WiFi.macAddress();
String MQTT_BASEPATH = config.mqttDomain + "/" + NODE_ID + "/";

WiFiManager wm;
WiFiClient wc;
PubSubClient MQTTClient(wc, config.mqttServer);
ESP8266WebServer server(80);

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

class ESPThing {
    private:
        bool fallback = false;
        int last_connect = 0;
        std::vector<Sender> senders;
        std::vector<Receiver> receivers;
        void setup() {
            log("setup");
            wm.setAPlist(config.APlist);

            // configure server
            server.on("/", handleRoot);
            server.onNotFound(handleNotFound);
            server.begin();
        }

    public:
        ESPThing(){
            setup();
        }

        ~ESPThing(){
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

        std::vector<Sender> getSenders() {
            return senders;
        }

        std::vector<Receiver> getReceivers() {
            return receivers;
        }

        void mqtt_callback(const MQTT::Publish& pub) {
            Serial.println("MQTT: " + pub.topic() + " = " + pub.payload_string());
            for(uint32_t x = 0; x < senders.size(); x++) {
                Sender s = senders[x];
                Serial.println(s.topic);
            }
        }

        void mqtt_loop() {
            if (MQTTClient.connected()) {
                MQTTClient.loop();
            } else {
                Serial.println("connecting MQTT");
                if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
                    Serial.println("MQTT connected");
                    std::function<void(const MQTT::Publish&)> callback = [=](const MQTT::Publish& publish) { this->mqtt_callback(publish); };
                    MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
                    MQTTClient.set_callback(callback);
                    MQTTClient.subscribe(MQTT_BASEPATH + "#");
                }
            }
        }

        void addSender(Sender s) {
            senders.push_back(s);
        }

        void addReceiver(Receiver r) {
            receivers.push_back(r);
        }
};

ESPThing Thing;

String sender_cb() {
    Serial.println("sender callback");
}

void receiver_cb(MQTT::Publish pub) {
    Serial.println("receiver callback");
}

void setup() {
    // For Debugging
    Serial.begin(115200);
    Thing.addSender(Sender("send_foobar", sender_cb));
    Thing.addReceiver(Receiver("recv_foobar", receiver_cb));
}

void loop() {
    Thing.loop();
}
