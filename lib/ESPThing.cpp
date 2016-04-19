/*
 * ESP Boilerplate OS - ESPThing.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "Arduino.h"
#include "WiFiManager.h"
#include "PubSubClient.h"
#include "ESP8266WebServer.h"
#include <json/json.h>
#include "ESPThingConfig.h"
#include "ESPThing.h"

String NODE_ID = WiFi.macAddress();
String MQTT_BASEPATH_WO_MAC = thingConfig.mqttDomain + "/";
String MQTT_BASEPATH_W_MAC = MQTT_BASEPATH_WO_MAC + NODE_ID + "/";
String MQTT_BASEPATH = thingConfig.includeMac ? MQTT_BASEPATH_W_MAC : MQTT_BASEPATH_WO_MAC;

WiFiManager wm;
WiFiClient wc;
PubSubClient MQTTClient(wc, thingConfig.mqttServer);
ESP8266WebServer server(80);

// ##################################################################################
// PUBLIC
// ##################################################################################

ESPThing::ESPThing(){
  setup();
}

ESPThing::~ESPThing(){
}

void ESPThing::loop(){
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
            if (last_connect + (1000 * thingConfig.reconnectTime) > millis()) {
                server_loop();
            } else {
                log("RECONNECT");
                fallback = false;
                last_connect = millis(); // reset timeout 
            }
        } else {
            // if not in fallback mode try to connect to network
            if (last_connect + (1000 * thingConfig.connectTime) > millis()) {
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

void ESPThing::addOutput(const Output &o) {
    outputs.push_back(o);
}

void ESPThing::addInput(const Input &i) {
    inputs.push_back(i);
}

// ##################################################################################
// PRIVATE
// ##################################################################################

void ESPThing::log(String message){
    if (Serial) {
        Serial.println(message);
    }
}

void ESPThing::handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
}

void ESPThing::handleNotFound() {
    server.send(404, "text/html", "File Not Found");
}

void ESPThing::mqtt_callback(const MQTT::Publish& pub) {
    for (auto &i : inputs) {
        if (pub.topic() == (MQTT_BASEPATH + i.topic)) {
            i.callback(pub);
        }
    }
}

void ESPThing::setup() {
    log("setup");
    wm.setAPlist(thingConfig.APlist);

    // thingConfigure server
    server.on("/", std::bind(&ESPThing::handleRoot, this));
    server.onNotFound(std::bind(&ESPThing::handleNotFound, this));
    server.begin();
}

void ESPThing::mqtt_loop() {
    if (MQTTClient.connected()) {
        MQTTClient.loop();
    } else {
        log("connecting MQTT");
        if (MQTTClient.connect(NODE_ID, MQTT_BASEPATH + "status", 0, true, "offline")) {
            log("MQTT connected");
            MQTTClient.set_callback(std::bind(&ESPThing::mqtt_callback, this, std::placeholders::_1));
            MQTTClient.subscribe(MQTT_BASEPATH + "#");
            MQTTClient.publish(MQTT_BASEPATH + "status", "online", true);
            if (thingConfig.friendlyName) {
                // if thingConfigured, publish our friendlyName
                MQTTClient.publish(MQTT_BASEPATH + "name", thingConfig.friendlyName, true);
            }
        }
    }
}

void ESPThing::server_loop() {
    if (WiFi.softAPIP()){
        server.handleClient();
    }
}






