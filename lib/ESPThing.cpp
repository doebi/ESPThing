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
String INTERNAL_MQTT_TOPIC_PREFIX_WO_MAC = thingConfig.mqttThingTopicPrefix + "/";
String INTERNAL_MQTT_TOPIC_PREFIX_WITH_MAC = INTERNAL_MQTT_TOPIC_PREFIX_WO_MAC + NODE_ID + "/";
String INTERNAL_MQTT_TOPIC_PREFIX = thingConfig.exposeMac ? INTERNAL_MQTT_TOPIC_PREFIX_WITH_MAC : INTERNAL_MQTT_TOPIC_PREFIX_WO_MAC;

static PubSubClient initMqttClient(WiFiClient &wc, PubSubClient &mqttThingClient) {
  if (thingConfig.mqttServer == NULL || thingConfig.mqttServer == thingConfig.mqttThingServer) {
    return mqttThingClient;
  } else {
    return PubSubClient(wc, thingConfig.mqttServer);
  }
}

static String buildTopic(String t, bool internal) {
    return internal
        ? INTERNAL_MQTT_TOPIC_PREFIX + t 
        : t;
}

WiFiManager wm;
WiFiClient wc;
PubSubClient mqttThingClient(wc, thingConfig.mqttThingServer);
PubSubClient mqttClient = initMqttClient(wc, mqttThingClient);
ESP8266WebServer server(80);

bool                  _fallback = false;
int                   _last_connect = 0;
std::vector<Output>   _mqtt_thingOutputs;
std::vector<Output>   _mqtt_outputs;

// ##################################################################################
// PUBLIC
// ##################################################################################

Input::Input(String t, void (*c)(const MQTT::Publish& pub)) {
  topic = t;
  callback = c;
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
    if (wifiConnected()) {
        _last_connect = millis(); // we still got connection (yay)
        mqtt_loop();
    } else {
        if (_fallback) {
            // in _fallback mode we handle server
            if (_last_connect + (1000 * thingConfig.reconnectTime) > millis()) {
                server_loop();
            } else {
                log("RECONNECT");
                _fallback = false;
                _last_connect = millis(); // reset timeout 
            }
        } else {
            // if not in _fallback mode try to connect to network
            if (_last_connect + (1000 * thingConfig.connectTime) > millis()) {
                wm.AutoConnect();
                delay(500);
            } else {
                // timeout reached we go to _fallback
                log("TIMEOUT");
                _fallback = true;
                wm.FallbackAP();
                _last_connect = millis(); // reset timeout 
            }
        }
    }
}

void ESPThing::addOutput(const Output &o) {
    thingPublish(o.topic, o.loop, o.interval);
}

void ESPThing::addInput(const Input &i) {
    thingSubscribe(i.topic, i.callback);
}

void ESPThing::thingSubscribe(String t, void (*c)(const MQTT::Publish& pub)) {
    mqtt_subscribe(t,c,true);
}

void ESPThing::thingPublish(String t, void (*l)(String * msg), int i) {
    mqtt_publish(t,l,i,true);
}

void ESPThing::subscribe(String t, void (*c)(const MQTT::Publish& pub)) {
    mqtt_subscribe(t,c,false);
}

void ESPThing::publish(String t, void (*l)(String * msg), int i) {
    mqtt_publish(t,l,i,false);
}

bool ESPThing::wifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool ESPThing::mqttClientConnected() {
  return mqttClient.connected();
}

bool ESPThing::mqttThingClientConnected() {
  return mqttThingClient.connected();
}

// ##################################################################################
// PRIVATE
// ##################################################################################

void ESPThing::log(String message){
    if (Serial) {
        Serial.println(message);
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

void ESPThing::handleRoot() {
    server.send(200, "text/html", "<h1>You are connected</h1>");
}

void ESPThing::handleNotFound() {
    server.send(404, "text/html", "File Not Found");
}

void ESPThing::server_loop() {
    if (WiFi.softAPIP()){
        server.handleClient();
    }
}

bool ESPThing::singleMqttClient() {
   return &mqttClient == &mqttThingClient;
}

void ESPThing::mqtt_loop() {
    // internal (thing) mqtt client
    if (mqttThingClient.connected()) {
        mqttThingClient.loop();
        mqtt_publishLoop(_mqtt_thingOutputs, mqttThingClient);
    } else {
        log("connecting internal MQTT");
        if (mqttThingClient.connect(NODE_ID, INTERNAL_MQTT_TOPIC_PREFIX + "status", 0, true, "offline")) {
            log("internal MQTT connected");
            mqttThingClient.set_callback(std::bind(&ESPThing::mqtt_callback, this, std::placeholders::_1, this->_mqtt_thingInputs));
            mqttThingClient.subscribe(INTERNAL_MQTT_TOPIC_PREFIX + "#");
            mqttThingClient.publish(INTERNAL_MQTT_TOPIC_PREFIX + "status", "online", true);
            if (thingConfig.friendlyName) {
                // if configured, publish our friendlyName
                mqttThingClient.publish(INTERNAL_MQTT_TOPIC_PREFIX + "name", thingConfig.friendlyName, true);
            }
        }
    }

    // external mqtt client
    if (!singleMqttClient()) {
        if (mqttClient.connected()) {
            mqttClient.loop();
            mqtt_publishLoop(_mqtt_outputs, mqttClient);
        } else {
            log("connecting external MQTT");
            if (mqttClient.connect(NODE_ID)) {
                log("external MQTT connected");
                mqttThingClient.set_callback(std::bind(&ESPThing::mqtt_callback, this, std::placeholders::_1, this->_mqtt_inputs));
            }
        }
    }    
}

void ESPThing::mqtt_callback(const MQTT::Publish& pub, const std::vector<Input> &inputs) {
    for (auto &i : inputs) {
        if (pub.topic() == i.topic) {
            i.callback(pub);
        }
    }
}

void ESPThing::mqtt_publishLoop(std::vector<Output> &outputs, PubSubClient &mqttClient) {
    String msg;
    int now = millis();
    for (auto &o : outputs) {
        if (o.last_run + o.interval <= now) {
            o.loop(&msg);
            if (msg != NULL) {
                mqttClient.publish(o.topic, msg);
            }
            o.last_run = now;
        }
    }
}

void ESPThing::mqtt_subscribe(String t, void (*c)(const MQTT::Publish& pub), bool internal) {
    String topic = buildTopic(t, internal);
    Input input = Input(topic, c);

    if (internal) {
        _mqtt_thingInputs.push_back(input);
    } else {
        mqttClient.subscribe(topic);
        
        if (singleMqttClient()) {
            _mqtt_thingInputs.push_back(input);
        } else {  
            _mqtt_inputs.push_back(input);
        }
    } 
}

void ESPThing::mqtt_publish(String t, void (*l)(String * msg), int i, bool internal) {
    Output output = Output(buildTopic(t,internal),l,i);
    if (internal || singleMqttClient()) {
        _mqtt_thingOutputs.push_back(output);
    } else {
        _mqtt_outputs.push_back(output);
    }
}



