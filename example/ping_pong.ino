/*
 * ESP Boilerplate OS - main.cpp
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "ESPThing.h"

ESPThing Thing;
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
    Serial.begin(115200);
    
	//Thing.addOutput(Output("pong", pong_loop));
    //Thing.addOutput(Output("heartbeat", heartbeat_loop, 1000 * 60 * 3));
    //Thing.addInput(Input("ping", ping_cb));
	
	Thing.thingPublish("pong", pong_loop);
    Thing.thingPublish("heartbeat", heartbeat_loop, 1000 * 60 * 3);
    Thing.thingSubscribe("ping", ping_cb);
}

void loop() {
    Thing.loop();
}
