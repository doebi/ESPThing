# ESPBoilerplate
### An IoT Boilerplate for ESP
The Basic Idea is to provide a working Firmware for ESP, where once can manually define **Senders** and **Receivers**.

### Receivers
Receivers listen on a specific topic for incoming messages and execute the supplied callback function once a new message arrives.

**Usage**
```arduino
void callback(MQTT::Publish pub) {
  //do something
}

App.addReceiver("topic", callback);
```

### Senders
Work the other way round. You implement a tryPublish method, which is executed in loop().
When this method returns a String this String is sent as message to the receivers topic.

**Usage**

```arduino
String tryPublish() {
    if (digitalRead(PIN) != PIN_STATE) {
        PIN_STATE = !PIN_STATE;
        if (PIN_STATE){
            return "YAY";
        } else {
            return "NEY";
        }
    }
}

App.addSender("bar", tryPublish);
```
