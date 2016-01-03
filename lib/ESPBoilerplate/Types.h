/*
 * ESP Boilerplate OS - Types.h
 * (c) by Christoph (doebi) Döberl
 *
 */

class Receiver {
    public:
        Receiver(String t, void (*c)(String topic, String message)){
            topic = t;
            callback = c;
        }
        String topic;
        void (*callback)(String topic, String message);
};
