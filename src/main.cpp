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
        { "Uplink", "level3support" },
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

void mqtt_callback(const MQTT::Publish& pub);

void server_loop() {
    if (WiFi.softAPIP()){
        server.handleClient();
    }
}

void handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
}

void handleNotFound() {
    server.send(404, "text/html", "File Not Found");
}

class Input {
    public:
        Input(){};
        Input(String t, void (*c)(const MQTT::Publish& pub)){
            topic = t;
            callback = c;
        }
        String topic;
        void (*callback)(const MQTT::Publish& pub);
};

class Output {
    public:
        Output(){};
        Output(String t, void (*l)(String * msg)){
            topic = t;
            loop = l;
            interval = 0;
        };
        Output(String t, void (*l)(String * msg), int i){
            topic = t;
            loop = l;
            interval = i;
        }
        String topic;
        void (*loop)(String * msg);
        int interval;
        int last_run = 0;
};

class ESPThing {
    private:
        bool fallback = false;
        int last_connect = 0;
        std::vector<Output> outputs;
        std::vector<Input> inputs;
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
                String msg;
                int now = millis();
                for (auto &o : outputs) {
                    if (o.last_run + o.interval <= now) {
                        o.loop(&msg);
                        if (msg != NULL) {
                            MQTTClient.publish(MQTT_BASEPATH + o.topic, msg);
                        }
                        o.last_run = now;
                    }
                }
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

        std::vector<Output> getOutputs() {
            return outputs;
        }

        std::vector<Input> getInputs() {
            return inputs;
        }

        void mqtt_loop() {
            if (MQTTClient.connected()) {
                MQTTClient.loop();
            } else {
                log("connecting MQTT");
                if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
                    log("MQTT connected");
                    MQTTClient.set_callback(mqtt_callback);
                    MQTTClient.subscribe(MQTT_BASEPATH + "#");
                    MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
                    if (config.friendlyName) {
                        // if configured, publish our friendlyName
                        MQTTClient.publish(MQTT_BASEPATH + "name", config.friendlyName, true);
                    }
                }
            }
        }

        void addOutput(const Output &o) {
            outputs.push_back(o);
        }

        void addInput(const Input &i) {
            inputs.push_back(i);
        }
};

ESPThing Thing;

void mqtt_callback(const MQTT::Publish& pub) {
    std::vector<Input> inputs = Thing.getInputs();
    for (auto &i : inputs) {
        if (pub.topic() == (MQTT_BASEPATH + i.topic)) {
            i.callback(pub);
        }
    }
}

/* ************* Thing End *************** */
/* add your Thing specific code below here */
/* *************************************** */

bool ping = false;
String ping_msg = "";

void pong_loop(String * msg) {
    if (ping) {
        ping = false;
        *msg = ping_msg;
    }
}

void heartbeat_loop(String * msg) {
    *msg = String(millis());
}

void ping_cb(const MQTT::Publish& pub) {
    ping_msg = pub.payload_string();
    ping = true;
}

void setup() {
    Thing.addOutput(Output("pong", pong_loop));
    Thing.addOutput(Output("heartbeat", heartbeat_loop, 1000 * 60 * 3));
    Thing.addInput(Input("ping", ping_cb));
}

void loop() {
    Thing.loop();
}
