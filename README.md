# ESPThing
### An IoT Boilerplate for ESP
The Basic Idea is to provide a working Firmware for ESP, where one can manually define **Inputs** and **Outputs**.
Additionally it provides an easy way of reconfiguration without the need to reflash via a webinterface.

### Input
An Input listen on a specific topic for incoming messages and execute the supplied callback function once a new message arrives.

### Output
Work the other way round. You implement a method, which is executed in loop().
When this method defines a String at msg, it is sent as message to the outputs topic.
Optionally you can add a third parameter to the Output class. An interval in milliseconds in which the loop method should be ecexuted. This defaults to 0.

### Example
In this small exmaple we define an Input **ping** and an Output **pong**.
When we receive a ping on topic ping, we send a reply on topic pong.
And a **heartbeat** is send every 3 minutes.

```arduino

bool ping = false;
String ping_msg = "";

void pong_loop(String * msg) {
    if (ping) {
        ping = false;
        *msg = ping_msg;
    }
}

void ping_cb(const MQTT::Publish& pub) {
    ping_msg = pub.payload_string();
    ping = true;
}

void heartbeat_loop(String * msg) {
    *msg = String(millis());
}

void setup() {
    Thing.addOutput(Output("pong", pong_loop));
    Thing.addOutput(Output("heartbeat", heartbeat_loop, 1000 * 60 * 3));
    Thing.addInput(Input("ping", ping_cb));
}

void loop() {
    Thing.loop();
}
```

The topics for this example would be:

```
domain/FF:FF:FF:FF:FF:FF/ping
domain/FF:FF:FF:FF:FF:FF/pong
```

### Reconfiguration
If the ESP is unable to connect to one of it's configured APs it falls back to AP Mode. The user can then connect to the AP and reconfigure settings.

* add/delete WifiAPs (ssid, password)
* edit MQTT host, domain, topic

### MQTT Topic Pattern
Each thing gets a unique MQTT Topic Base consisting of a domain and the ESP's MAC 
Every Input and Output's topic gets appended to this base.

```
<DOMAIN>/<MAC>/status    online|offline
<DOMAIN>/<MAC>/<name>    <message>
```
