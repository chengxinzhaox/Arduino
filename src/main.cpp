#include <Arduino.h>

boolean pushButton1;
boolean pushButton2;
int ledPin = 9;
int brightness = 128;

void setup() {
    pinMode(2, INPUT_PULLUP);
    pinMode(8, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
}


void loop() {
    pushButton1 = digitalRead(2);
    pushButton2 = digitalRead(8);

    if (!pushButton1 && brightness > 0) {
        brightness--;
    } else if (!pushButton2 && brightness < 255) {
        brightness++;
    }

    analogWrite(ledPin, brightness);
    Serial.println(brightness);
    delay(10);
}
