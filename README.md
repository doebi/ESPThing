# ESPThing
### An IoT Boilerplate for ESP
The Basic Idea is to provide a working Firmware for ESP, where one can manually define **Inputs** and **Outputs**.
Additionally it provides an easy way of reconfiguration without the need to reflash via a webinterface.

### Input
An Input listen on a specific topic for incoming messages and execute the supplied callback function once a new message arrives.

### Senders
Work the other way round. You implement a tryPublish method, which is executed in loop().
When this method returns a String this String is sent as message to the receivers topic.

### Example
In this small exmaple we define an Input **ping** and an Output **pong**.
When we receive a ping on topic ping, we send a reply on topic pong.

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

void setup() {
    Thing.addOutput(Output("pong", pong_loop));
    Thing.addInput(Input("ping", ping_cb));
}

void loop() {
    Thing.loop();
}
```

The topics for this this example would be:

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
