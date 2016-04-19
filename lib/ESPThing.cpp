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
String PREFIX_WO_MAC = thingConfig.mqttDomain + "/";
String PREFIX_WITH_MAC = PREFIX_WO_MAC + NODE_ID + "/";
String INTERNAL_MQTT_TOPIC_PREFIX = thingConfig.exposeMac ? PREFIX_WITH_MAC : PREFIX_WO_MAC;

WiFiManager wm;
WiFiClient wc;
PubSubClient MQTTClient(wc, thingConfig.mqttServer);
ESP8266WebServer server(80);


String buildTopic(String t, bool internal) {
    return internal
        ? INTERNAL_MQTT_TOPIC_PREFIX + t
        : t;
}

// ##################################################################################
// PUBLIC
// ##################################################################################

Input::Input(String t, void (*c)(const MQTT::Publish& pub)) {
  topic = t;
  callback = c;
}

Output::Output(String t, void (*l)(String * msg)){
    Output(t,l,0);
}

Output::Output(String t, void (*l)(String * msg), int i){
    topic = t;
    loop = l;
    interval = i;
}

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
                    MQTTClient.publish(o.topic, msg);
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
    Output newOutput = Output(buildTopic(o.topic,true), o.loop, o.interval);
    outputs.push_back(newOutput);
}

void ESPThing::addInput(const Input &i) {
    Input newInput = Input(buildTopic(i.topic, true), i.callback);
    inputs.push_back(newInput);
}

void ESPThing::thingSubscribe(String t, void (*c)(const MQTT::Publish& pub)) {
    subscribe(t,c,true);
}

void ESPThing::thingPublish(String t, void (*l)(String * msg)) {
    thingPublish(t,l,0);
}

void ESPThing::thingPublish(String t, void (*l)(String * msg), int i) {
    publish(t,l,i,true);
}

void ESPThing::subscribe(String t, void (*c)(const MQTT::Publish& pub)) {
    subscribe(t,c,false);
}

void ESPThing::publish(String t, void (*l)(String * msg)) {
    publish(t,l,0);
}

void ESPThing::publish(String t, void (*l)(String * msg), int i) {
    publish(t,l,i,false);
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
        if (pub.topic() == i.topic) {
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
        if (MQTTClient.connect(NODE_ID, INTERNAL_MQTT_TOPIC_PREFIX + "status", 0, true, "offline")) {
            log("MQTT connected");
            MQTTClient.set_callback(std::bind(&ESPThing::mqtt_callback, this, std::placeholders::_1));
            MQTTClient.subscribe(INTERNAL_MQTT_TOPIC_PREFIX + "#");
            MQTTClient.publish(INTERNAL_MQTT_TOPIC_PREFIX + "status", "online", true);
            if (thingConfig.friendlyName) {
                // if configured, publish our friendlyName
                MQTTClient.publish(INTERNAL_MQTT_TOPIC_PREFIX + "name", thingConfig.friendlyName, true);
            }
        }
    }
}

void ESPThing::server_loop() {
    if (WiFi.softAPIP()){
        server.handleClient();
    }
}

void ESPThing::subscribe(String t, void (*c)(const MQTT::Publish& pub), bool internal) {
    String topic = buildTopic(t, internal);
    if (!internal) {
        MQTTClient.subscribe(topic);
    }
    inputs.push_back(Input(topic, c));
}

void ESPThing::publish(String t, void (*l)(String * msg), int i, bool internal) {
    outputs.push_back(Output(buildTopic(t,internal),l,i));
}



