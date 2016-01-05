/**
 *
 * @file WiFiManager.h
 * @date 16.05.2015
 * @author Markus Sattler
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the esp8266 core for Arduino environment.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef WIFIMANAGER_H_
#define WIFIMANAGER_H_

#include "ESP8266WiFi.h"
#undef min
#undef max
#include <vector>

// #define DEBUG_WIFI_MULTI(...) Serial1.printf( __VA_ARGS__ )

#ifndef DEBUG_WIFI_MULTI
#define DEBUG_WIFI_MULTI(...)
#endif

typedef struct {
        char * ssid;
        char * passphrase;
} WifiAPlist_t;

class WiFiManager {
    public:
        WiFiManager();
        ~WiFiManager();

        bool addAP(const char* ssid, const char *passphrase = NULL);
        void loop();

        wl_status_t AutoConnect(void);
        wl_status_t FallbackAP(void);

    private:
        std::vector<WifiAPlist_t> APlist;
        bool APlistAdd(const char* ssid, const char *passphrase = NULL);
        void APlistClean(void);

};

#endif /* WIFIMANAGER_H_ */
