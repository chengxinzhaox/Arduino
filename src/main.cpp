#include <Arduino.h>

int PIN = 5;

void setup() {
    pinMode(PIN, OUTPUT);
}

void loop() {
    digitalWrite(PIN, HIGH);
    delay(500);
    digitalWrite(PIN, LOW);
    delay(1000);
}
