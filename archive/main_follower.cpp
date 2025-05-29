#include <Wire.h>

#define SLAVE_ADDR 0x09

int raw_reading = 1000;
void receiveEvent(int bytes)
{
    while (Wire.available())
    {
        String msg = Wire.readStringUntil(',');
        raw_reading = atoi(msg.c_str());
        Serial.println(msg);
    }
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(SLAVE_ADDR); // follower mode
    Wire.onReceive(receiveEvent);
    pinMode(LED_BUILTIN, OUTPUT);
}

int min_delay = 10;
int max_delay = 1000;
int blink_time = 3;
void loop()
{
    // do nothing, wait for messages
    digitalWrite(LED_BUILTIN, HIGH);
    delay(blink_time);
    digitalWrite(LED_BUILTIN, LOW);
    delay(map(raw_reading, 0, 1023, min_delay, max_delay));
}
