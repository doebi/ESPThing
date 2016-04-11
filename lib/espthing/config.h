/*
 * ESP Boilerplate OS - config.h
 * (c) by Christoph (doebi) Döberl
 *
 */
#include "Arduino.h"
#include "WiFiManager.h"
#include <vector>

typedef struct {
    std::vector<WifiAPlist_t> APlist;
    String friendlyName;
    String mqttServer;
    String mqttDomain;
    int connectTime;
    int reconnectTime;
    bool includeMac;
} thing_config_t;

thing_config_t config = {
    {
        { "ssid", "password" }
    },
    "mqtt-device-name",
    "mqtt.devlol.org",
    "devlol/things",
    60,
    60 * 15,
    true
};